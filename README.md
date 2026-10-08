# APP_CPAS_SANTE

Dépôt commun des projets santé.

| Dossier | Contenu |
|---|---|
| `APP/` | APP : dispositif anti-sédentarité (Raspberry Pi Pico 2 W + LSM6DSOX) |
| `CPAS/` | CPAS : projet des autres étudiants |

## Travailler en équipe

Les règles et réglages sont dans le dépôt : un `git clone` suffit pour les avoir.

- `.claude/` : consignes de Claude Code (`CLAUDE.md`), hooks (compilation, tests, règles Git), skill `git`.
- `.vscode/`, `.clangd`, `.editorconfig` : réglages de l'éditeur (UTF-8, LF, 4 espaces).

À installer sur chaque ordinateur (macOS ou Linux ; sous Windows, utiliser WSL) :

1. `git`, `jq`, `cmake`, `ninja`, `python3`, la CLI GitHub `gh` (`gh auth login`).
2. Le SDK Pico 2.2.0 dans `~/.pico-sdk/sdk` et le compilateur `arm-none-eabi-gcc`
   (extension VS Code « Raspberry Pi Pico » ou installation manuelle).
3. Ouvrir le dépôt dans Claude Code et accepter les réglages du projet (sinon les hooks ne tournent pas).

Flux Git : `main` <- `develop` <- `feature/<nom>` ou `fix/<nom>`. Aucun commit sur `main`
ni `develop`, 4 fichiers max par commit. Détails : `.claude/skills/git/SKILL.md`.
