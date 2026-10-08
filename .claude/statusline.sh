#!/bin/bash
# Barre d'état de Claude Code (sous la zone de saisie). Claude Code envoie les infos
# de la session en JSON sur l'entrée standard ; le script affiche 2 lignes :
#   1. modèle | effort | branche ~fichiers modifiés | ↑ à pousser ↓ à récupérer | wkx worktrees | PR | coût
#   2. contexte utilisé | limite 5 h | durée | lignes +/- | Pico branché ? | dernière CI GitHub

entree=$(cat)
champ() { jq -r "$1 // empty" <<<"$entree"; }

GRIS=$'\e[90m'; CYAN=$'\e[36m'; VERT=$'\e[32m'; JAUNE=$'\e[33m'; ROUGE=$'\e[31m'; FIN=$'\e[0m'
SEP="${GRIS} | ${FIN}"
dossier=$(champ '.workspace.current_dir // .cwd'); dossier=${dossier:-.}
git_cmd() { git -C "$dossier" --no-optional-locks "$@" 2>/dev/null; }

# Couleur selon un pourcentage : vert < 50, jaune < 80, rouge au-delà
couleur_pct() {
    if [ "$1" -ge 80 ]; then echo "$ROUGE"; elif [ "$1" -ge 50 ]; then echo "$JAUNE"; else echo "$VERT"; fi
}

# --- Ligne 1 : session et git ------------------------------------------------
segment_git() {
    local branche ecart derriere devant couleur modifs worktrees
    branche=$(git_cmd branch --show-current)
    [ -z "$branche" ] && { echo "${GRIS}hors git${FIN}"; return; }
    modifs=$(git_cmd status --porcelain | wc -l | tr -d ' ')
    [ "$modifs" != 0 ] && branche="${branche} ${JAUNE}~${modifs}"
    ecart=$(git_cmd rev-list --left-right --count '@{upstream}...HEAD' \
        || git_cmd rev-list --left-right --count 'origin/main...HEAD' || echo "0 0")
    read -r derriere devant <<<"$ecart"
    couleur=$VERT
    { [ "$devant" != 0 ] || [ "$derriere" != 0 ]; } && couleur=$JAUNE
    worktrees=$(git_cmd worktree list | wc -l | tr -d ' ')
    echo "${CYAN}${branche}${FIN}${SEP}${couleur}↑${devant} ↓${derriere}${FIN}${SEP}wkx${worktrees}"
}

effort=$(champ '.effort.level')
ligne1="$(champ '.model.display_name')${effort:+${SEP}${effort}}${SEP}$(segment_git)"
pr=$(champ '.pr.number')
[ -n "$pr" ] && ligne1+="${SEP}PR #${pr} $(champ '.pr.review_state')"
cout=$(champ '.cost.total_cost_usd'); cout=${cout:-0}
cout_txt=$(printf '$%.2f' "$cout")
[ "$(printf '%.0f' "$cout")" -ge 5 ] && cout_txt="${ROUGE}${cout_txt}${FIN}"
ligne1+="${SEP}${cout_txt}"

# --- Ligne 2 : contexte, usage, matériel, CI -----------------------------------
segment_contexte() {
    local pct plein barre
    pct=$(printf '%.0f' "$(champ '.context_window.used_percentage')" 2>/dev/null); pct=${pct:-0}
    plein=$((pct / 10))
    barre=$(printf '%*s' "$plein" '' | tr ' ' '#')$(printf '%*s' $((10 - plein)) '' | tr ' ' '.')
    echo "ctx $(couleur_pct "$pct")[${barre}] ${pct}%${FIN}"
}

segment_limite() {
    local pct
    pct=$(champ '.rate_limits.five_hour.used_percentage')
    [ -z "$pct" ] && return
    pct=$(printf '%.0f' "$pct")
    echo "${SEP}5h $(couleur_pct "$pct")${pct}%${FIN}"
}

segment_duree() {
    local ms min
    ms=$(champ '.cost.total_duration_ms'); min=$(( ${ms:-0} / 60000 ))
    printf '%s%dh%02d' "$SEP" $((min / 60)) $((min % 60))
}

segment_lignes() {
    echo "${SEP}${VERT}+$(champ '.cost.total_lines_added // 0')${FIN} ${ROUGE}-$(champ '.cost.total_lines_removed // 0')${FIN}"
}

segment_pico() {
    if ls /dev/cu.usbmodem* >/dev/null 2>&1; then echo "${SEP}${VERT}Pico branché${FIN}"
    elif [ -d /Volumes/RP2350 ]; then echo "${SEP}${JAUNE}Pico BOOTSEL${FIN}"
    else echo "${SEP}${GRIS}Pico absent${FIN}"; fi
}

# Dernière CI GitHub de la branche : lue dans un cache, rafraîchi toutes les 2 min par un
# processus détaché (gh met ~1 s à répondre : la barre ne doit jamais l'attendre)
segment_ci() {
    local branche cache etat
    command -v gh >/dev/null || return
    branche=$(git_cmd branch --show-current); [ -z "$branche" ] && return
    cache="${TMPDIR:-/tmp}/statusline_ci_$(echo "$dossier $branche" | cksum | cut -d' ' -f1)"
    if [ ! -f "$cache" ] || [ -n "$(find "$cache" -mmin +2 2>/dev/null)" ]; then
        touch "$cache"
        (cd "$dossier" && gh run list --branch "$branche" --limit 1 --json status,conclusion \
            -q '.[0] | if . == null then "" elif .status != "completed" then "en cours" else .conclusion end' \
            >"$cache.tmp" && mv "$cache.tmp" "$cache") </dev/null >/dev/null 2>&1 &
        disown 2>/dev/null
    fi
    etat=$(cat "$cache" 2>/dev/null)
    case "$etat" in
        success) echo "${SEP}CI ${VERT}ok${FIN}" ;;
        failure) echo "${SEP}CI ${ROUGE}échec${FIN}" ;;
        "") ;;
        *) echo "${SEP}CI ${JAUNE}${etat}${FIN}" ;;
    esac
}

ligne2="$(segment_contexte)$(segment_limite)$(segment_duree)$(segment_lignes)$(segment_pico)$(segment_ci)"

printf '%s\n%s\n' "$ligne1" "$ligne2"
