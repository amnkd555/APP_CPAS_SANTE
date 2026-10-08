#!/bin/bash
# Hook PreToolUse (Bash) : fait respecter les règles Git du projet (skill « git »)
# avant chaque « git commit », « git push », « git merge » et « gh pr merge » lancé par Claude.
#
# Branches : main (versions stables) <- develop (intégration) <- feature/*, fix/*... (travail).
# Commit refusé si : branche main ou develop (ou hors feature/, fix/, hotfix/, chore/, docs/, refactor/, test/),
#                    git add dans la même commande, plus de 4 fichiers, message hors format, emoji, jeton GitHub.
# Merge refusé si   : vers main depuis autre chose que develop, vers develop depuis autre chose
#                    qu'une branche de travail (git merge et gh pr merge).
# Push refusé si    : push forcé sur main ou develop.
#
# Code de sortie 2 = commande bloquée, le message est renvoyé à Claude.

set -u
entree=$(cat)
commande=$(jq -r '.tool_input.command // empty' <<<"$entree")
dossier=$(jq -r '.cwd // empty' <<<"$entree")
cd "${dossier:-${CLAUDE_PROJECT_DIR:-.}}" 2>/dev/null || exit 0

refuser() {
    echo "BLOQUE par .claude/hooks/verif_git.sh : $1" >&2
    exit 2
}

contient_emoji() {
    perl -CSD -ne 'if (/[\x{1F000}-\x{1FAFF}\x{2600}-\x{27BF}\x{2B00}-\x{2BFF}\x{FE0F}]/) { $t = 1 } END { exit($t ? 0 : 1) }'
}

grep -Eq 'git( -C [^ ]+)? (commit|push|merge)|gh pr merge' <<<"$commande" || exit 0
branche=$(git branch --show-current 2>/dev/null) || exit 0

TRAVAIL='^(feature|fix|hotfix|chore|docs|refactor|test)/'

# Merge autorisé : develop -> main, branche de travail -> develop. Rien d'autre vers main/develop.
verifier_merge() {
    case "$1" in
        main) [ "$2" = "develop" ] || refuser "merge vers main uniquement depuis develop (source : '$2')" ;;
        develop) grep -Eq "$TRAVAIL" <<<"$2" || refuser "merge vers develop uniquement depuis feature/, fix/... (source : '$2')" ;;
    esac
}

# --- Merge -------------------------------------------------------------------
if grep -Eq 'gh pr merge' <<<"$commande"; then
    pr=$(sed -E 's/.*gh pr merge//; s/&&.*//' <<<"$commande" | tr ' ' '\n' | grep -E '^[0-9]+$' | head -1)
    infos=$(gh pr view $pr --json baseRefName,headRefName -q '.baseRefName + " " + .headRefName' 2>/dev/null) \
        || refuser "pull request introuvable : preciser son numero"
    verifier_merge ${infos% *} ${infos#* }
fi
if grep -Eq 'git( -C [^ ]+)? merge' <<<"$commande"; then
    source=$(sed -E 's/.*git( -C [^ ]+)? merge//; s/&&.*//' <<<"$commande" | tr ' ' '\n' | grep -Ev '^-|^$|^"' | head -1)
    verifier_merge "$branche" "${source#origin/}"
fi

# --- Commit ------------------------------------------------------------------
if grep -Eq 'git( -C [^ ]+)? commit' <<<"$commande"; then
    grep -Eq "$TRAVAIL" <<<"$branche" \
        || refuser "commit interdit sur '$branche'. Creer une branche depuis develop : git switch -c feature/<nom> develop (ou fix/<nom>)"

    grep -Eq 'git( -C [^ ]+)? add|commit (.* )?(-a|-am|--all)( |$)' <<<"$commande" \
        && refuser "faire git add puis git commit dans deux commandes separees (pour compter les fichiers)"

    nb=$(git diff --cached --name-only | wc -l | tr -d ' ')
    [ "$nb" -gt 4 ] && refuser "$nb fichiers prepares, 4 max par commit. Decouper : git restore --staged <fichiers>"

    grep -Eq '(feat|fix|docs|refactor|test|chore|ci|build)(\([a-z_-]+\))?: ' <<<"$commande" \
        || refuser "message hors format. Attendu : type(portee): description (feat, fix, docs, refactor, test, chore, ci, build)"

    contient_emoji <<<"$commande" && refuser "emoji dans le message de commit"
    grep -Eqi 'co-authored-by|generated with' <<<"$commande" \
        && refuser "pas de Co-Authored-By ni de mention de Claude dans les commits"

    ajouts=$(git diff --cached -U0 | grep '^+' | grep -v '^+++')
    contient_emoji <<<"$ajouts" && refuser "emoji dans les fichiers prepares"
    grep -Eq 'gh[pousr]_[A-Za-z0-9]{20,}|github_pat_[A-Za-z0-9_]{20,}' <<<"$ajouts" \
        && refuser "jeton GitHub dans les fichiers prepares"
fi

# --- Push --------------------------------------------------------------------
if grep -Eq 'git( -C [^ ]+)? push' <<<"$commande"; then
    if grep -Eq 'git push.*(-f|--force)' <<<"$commande"; then
        { [ "$branche" = "main" ] || [ "$branche" = "develop" ] || grep -Eq '[ :/](main|develop)( |$)' <<<"$commande"; } \
            && refuser "jamais de push force sur main ou develop"
    fi
fi

exit 0
