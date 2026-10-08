#!/bin/bash
# Vérification complète du projet. Lancée par le hook Claude Code (PostToolUse sur Edit|Write),
# par la CI GitHub (.github/workflows/ci.yml) et à la main : ./APP/outils/scripts/verifier.sh </dev/null
#
# 1. Compile le firmware (toutes les cibles CMake)            -> bloquant
# 2. Compile et exécute les tests C sur le Mac (outils/tests/test_*.c) -> bloquant
#    la ligne « // SOURCES: a.c b.c » liste les fichiers à compiler avec le test
#    (chemins relatifs à la racine)
# 3. Vérifie la syntaxe des scripts Python                    -> bloquant
# 4. Vérifie la règle des 30 lignes max par fonction          -> avertissement
# 5. Vérifie la règle des 100 lignes max par fichier de code  -> avertissement
#
# Code de sortie 2 = échec : le message est renvoyé à Claude pour qu'il corrige.

set -u
# Racine du code : dossier APP/ (le code y est, les outils sont dans APP/outils/)
RACINE="$(cd "$(dirname "$0")/../.." && pwd)"
BUILD=outils/build
cd "$RACINE" || exit 0

# Appelé par le hook : ne réagit qu'aux fichiers de code du projet (hors outils/build/)
if [ ! -t 0 ]; then
    fichier=$(jq -r '.tool_input.file_path // .tool_response.filePath // empty' 2>/dev/null)
    case "$fichier" in
        "") ;;
        "$RACINE"/outils/build/*) exit 0 ;;
        "$RACINE"/*.c|"$RACINE"/*.h|"$RACINE"/*CMakeLists.txt|"$RACINE"/*.py|"$RACINE"/outils/scripts/verifier.sh) ;;
        *) exit 0 ;;
    esac
fi

echecs=""
avertissements=""
journal="$(mktemp)"
trap 'rm -f "$journal"; rm -rf "$RACINE/$BUILD/tests_hote"' EXIT

# --- 1. Firmware -----------------------------------------------------------
# SDK et compilateur ARM : installation du Mac par défaut, ceux de la CI sinon
export PICO_SDK_PATH="${PICO_SDK_PATH:-$HOME/.pico-sdk/sdk}"
chaine_arm="$HOME/.pico-sdk/arm-gnu-toolchain-14.2.rel1-darwin-arm64-arm-none-eabi"
[ -d "$chaine_arm" ] && export PICO_TOOLCHAIN_PATH="${PICO_TOOLCHAIN_PATH:-$chaine_arm}"
if [ ! -f $BUILD/build.ninja ]; then
    cmake -S outils -B $BUILD -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON >"$journal" 2>&1
fi
if ! cmake --build $BUILD >"$journal" 2>&1; then
    echecs+="ÉCHEC compilation firmware :"$'\n'"$(grep -E 'error|Error' "$journal" | head -15)"$'\n'
fi
avert_fw=$(grep -E "^$RACINE/[a-z_]+\.c.*warning:" "$journal" | sort -u | head -10)
[ -n "$avert_fw" ] && avertissements+="Avertissements de compilation :"$'\n'"$avert_fw"$'\n'

# --- 2. Tests unitaires C (sur le Mac) ---------------------------------------
mkdir -p $BUILD/tests_hote
nb_tests_c=0
while IFS= read -r test; do
    nb_tests_c=$((nb_tests_c + 1))
    nom=$(basename "$test" .c)
    sources=$(grep -m1 '^// SOURCES:' "$test" | sed 's|^// SOURCES:||')
    # shellcheck disable=SC2086
    if ! cc -std=c11 -Wall -Wextra -Werror -I. -o "$BUILD/tests_hote/$nom" \
            "$test" $sources -lm >"$journal" 2>&1; then
        echecs+="ÉCHEC compilation $test :"$'\n'"$(head -15 "$journal")"$'\n'
    elif ! "$BUILD/tests_hote/$nom" >"$journal" 2>&1; then
        echecs+="ÉCHEC $test :"$'\n'"$(tail -15 "$journal")"$'\n'
    fi
done < <(find outils/tests -name 'test_*.c' 2>/dev/null | sort)

# --- 3. Python ----------------------------------------------------------------
for script in outils/scripts/*.py; do
    [ -f "$script" ] || continue
    if ! python3 -m py_compile "$script" 2>"$journal"; then
        echecs+="ÉCHEC syntaxe $script :"$'\n'"$(cat "$journal")"$'\n'
    fi
done

# --- 4. Règle : 30 lignes max par fonction -------------------------------------
# Une fonction commence par une ligne non indentée finissant par « { » et se termine
# par « } » en colonne 0 ; on compte les lignes du corps.
trop_longues=$(find . -maxdepth 1 -name '*.c' | sort | while read -r f; do
    awk -v f="$f" '
        /^[A-Za-z_].*\)[ \t]*\{[ \t]*$/ { nom = $0; debut = NR; next }
        debut && /^\}/ { n = NR - debut - 1; if (n > 30) printf "%s:%d %d lignes : %s\n", f, debut, n, nom; debut = 0 }
    ' "$f"
done)
[ -n "$trop_longues" ] && avertissements+="Fonctions de plus de 30 lignes (règle CLAUDE.md) :"$'\n'"$trop_longues"$'\n'

# --- 5. Règle : 100 lignes max par fichier de code ------------------------------
gros_fichiers=$(git ls-files '*.c' '*.h' '*.py' '*.sh' 2>/dev/null | while read -r f; do
    n=$(wc -l <"$f" | tr -d ' ')
    [ "$n" -gt 100 ] && echo "$f : $n lignes"
done)
[ -n "$gros_fichiers" ] && avertissements+="Fichiers de plus de 100 lignes (règle CLAUDE.md) :"$'\n'"$gros_fichiers"$'\n'

# --- Bilan --------------------------------------------------------------------
if [ -n "$echecs" ]; then
    printf '%s\n%s' "$echecs" "$avertissements" >&2
    exit 2
fi
resume="Tests OK : firmware compilé, $nb_tests_c test(s) C passé(s)."
if [ -n "$avertissements" ]; then
    # Succès avec avertissements : renvoyés à Claude comme contexte, sans bloquer
    jq -n --arg ctx "$resume"$'\n'"$avertissements" \
        '{hookSpecificOutput: {hookEventName: "PostToolUse", additionalContext: $ctx}}'
else
    echo "$resume"
fi
exit 0
