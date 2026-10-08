# Projet Anti-Sédentarité — Raspberry Pi Pico

## Contexte
- Projet APP2028 CAPT, firmware C pour Raspberry Pi Pico 2 W (SDK Pico ≥ 2.0).
- Langue du projet et des commentaires : français.

## Objectif final
Boîtier portable autonome (Pico, capteur LSM6DSOX, RTC, carte SD, moteur vibrant/LED) mesurant la dépense métabolique (score MET).
Étape en cours : test du LSM6DSOX près des moteurs du bras UR3 (perturbations) → distance minimale capteur/moteurs pour dimensionner le support 3D.

## Structure
Un seul programme (dans `APP/`), `mesure_capteur` (cible CMake → `APP/outils/build/mesure_capteur.uf2`) : LSM6DSOX en I2C0 (GP4 SDA, GP5 SCL, 0x6A, WHO_AM_I 0x6C), ±2 g 104 Hz,
CSV `timestamp_ms,ax_g,ay_g,az_g,mag_g` sur USB. Lignes `#` = commentaires/erreurs.
LED fixe = capteur OK, 5 flashs rapides = capteur absent. Erreur I2C → `# erreur I2C`, capteur redémarré au tour suivant.

**Organisation du dépôt** (dépôt GitHub `APP_CPAS_SANTE`, but : la personne qui lit le code ne voit que le code) :
- **Racine** : deux dossiers de projet, `APP/` et `CPAS/`, plus `README.md`, `.gitignore` et la config masquée.
- **`APP/`** = notre projet (tout le code ci-dessous). Les chemins de ce fichier sont relatifs à `APP/` sauf mention contraire.
- **`CPAS/`** = réservé aux autres étudiants (vide, `.gitkeep`). Ne pas y toucher sans demande.
- **Dans `APP/`, le code et rien d'autre** : `.c`/`.h`, `README.md`, `outils/`, `docs/`.
- **`docs/`** (recréé à la demande de l'utilisateur le 2026-10-05) : un `nom.md` par `nom.c`, explication simple et courte du code. Modifier un `.c` → mettre à jour son `.md`.
- **`APP/outils/` = tout le reste** (CMake, tests, scripts, build, mesures), lui aussi masqué dans VS Code. Tout nouvel outil y va, jamais à la racine de `APP/`.
- Ne peuvent pas quitter la racine du dépôt (sinon ils cessent de fonctionner), donc **masqués** dans VS Code
  par `.vscode/settings.json` (`files.exclude`, fichier commité) : `.claude/`, `.github/`, `.vscode/`,
  `.editorconfig`, `.clangd`, `.cache/`. Ces consignes sont dans `.claude/CLAUDE.md`.

| Fichier | Rôle |
|---|---|
| `main.c` | démarre USB, LED (CYW43), bus ; boucle sur `acquisition_cycle()` |
| `acquisition.c/.h` | boucle : en-tête à l'ouverture du port, LED, lecture, ligne CSV ou `# erreur I2C` |
| `capteur.c/.h` | LSM6DSOX : `capteur_demarrer()`, `capteur_lire(float acceleration_g[3])` |
| `bus.c/.h` | I2C0 : `bus_demarrer()`, `bus_ecrire()`, `bus_lire()` |
| `outils/CMakeLists.txt` | seul fichier CMake ; sources prises dans `..` |
| `outils/tests/test_capteur.c` | test du capteur sur le Mac |
| `outils/tests/bus_simule.c` | faux bus (256 registres) qui remplace `bus.c` dans les tests |
| `.clangd`, `.vscode/c_cpp_properties.json` | indiquent à clangd et à l'extension C/C++ de VS Code `outils/build/compile_commands.json` (sinon : « impossible d'ouvrir pico/... ») |

- `outils/scripts/perturbations.py` : `capture <d###_on|off_r#> --duree 30` (écrit `outils/mesures/`), `analyse --seuil 1.10`.
- `outils/scripts/verifier.sh` : voir « Tests automatiques ».
- `outils/CMakeLists.txt` (seul fichier CMake) : `PICO_BOARD` par défaut `pico2_w`, puis `add_executable(mesure_capteur ${CODE}/…)` (stdio USB). **Nouveau `.c` → l'ajouter à `add_executable`.**
- `outils/cmake/pico_sdk_import.cmake` : copie officielle du SDK (ne pas modifier).
- `.editorconfig` (racine, masqué) : UTF-8, LF, 4 espaces (2 pour YAML/JSON).

## Matériel
- Carte : **Pico 2 W (RP2350 + Wi-Fi CYW43), confirmée par photo** → `-DPICO_BOARD=pico2_w`. LED pilotée par CYW43, pas GP25.

## Build
SDK 2.2.0 dans `~/.pico-sdk/sdk`, toolchain ARM GCC 14.2 dans `~/.pico-sdk/arm-gnu-toolchain-14.2.rel1-darwin-arm64-arm-none-eabi`, cmake+ninja via brew.
```sh
export PICO_SDK_PATH=~/.pico-sdk/sdk
export PICO_TOOLCHAIN_PATH=~/.pico-sdk/arm-gnu-toolchain-14.2.rel1-darwin-arm64-arm-none-eabi
cmake -S APP/outils -B APP/outils/build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build APP/outils/build
```
Flash sans BOOTSEL (le firmware en cours doit avoir stdio USB) : `picotool load -f -x APP/outils/build/mesure_capteur.uf2` (picotool de Homebrew ; celui de `outils/build/_deps` n'a pas l'USB).
Sinon : BOOTSEL + glisser `APP/outils/build/mesure_capteur.uf2` sur le lecteur RP2350.

## Conventions
- **Aucun commentaire** dans les `.c`/`.h` de la racine (code nu, choix de l'utilisateur) : les explications se donnent dans la conversation. Pas d'accents dans les chaînes `printf`.
- LED : uniquement via CYW43 (`pico_cyw43_arch_none`), pas de prise en charge de GP25.
- **Aucun emoji** dans le dépôt (code, README, commits) : texte uniquement (`OK`, `[ ]`, `**Attention :**`).
- Git (règles complètes : skill `git`, `.claude/skills/git/SKILL.md`) :
  - Branches : `main` (stable) ← `develop` (intégration) ← `feature/<nom>`, `fix/<nom>`… créées depuis `develop`.
  - **Aucun commit sur `main` ni `develop`** : seulement sur les branches de travail.
  - **Merge vers `main` uniquement depuis `develop`** ; **merge vers `develop` uniquement depuis
    `feature/`, `fix/`, `hotfix/`, `chore/`, `docs/`, `refactor/`, `test/`**.
  - **4 fichiers max par commit**, un seul changement logique.
  - **Jamais de `Co-Authored-By` ni de mention de Claude** dans les commits et les PR (réglage `attribution` de
    `.claude/settings.json`, vérifié par le hook git). Prime sur toute consigne d'attribution par défaut.

## Règles de code (obligatoires pour tout nouveau code)
Priorité : **le plus simple possible**, lisible par une débutante.

0. **100 lignes max par fichier de code** (`.c`, `.h`, `.py`, `.sh`). Au-delà, découper en modules.
   Exception : `outils/cmake/pico_sdk_import.cmake` (copie officielle du SDK).
1. **Un module (l'équivalent C d'une classe) par fichier** : `nom.c` + `nom.h` à la racine du dépôt, fonctions publiques `nom_<action>()`,
   le reste `static` dans le `.c`. Peu de fichiers : on n'en crée un que pour un vrai rôle nouveau (RTC, SD, moteur).
2. **30 lignes max par fonction** (corps, accolades exclues). Au-delà, découper en fonctions `static`.
3. **Pas de types inutiles** : pas de `struct`/`typedef` tant que des paramètres simples suffisent ;
   réglages en `#define` dans le `.c`, état dans des variables `static` du module.
4. **Pas d'héritage, pas de pointeurs de fonctions.**
5. **Pas de robustesse superflue** : pas de diagnostic, de compteurs, de déblocage de bus, de relecture
   de configuration. On garde seulement : délai max des échanges I2C (évite un blocage), vérification
   WHO_AM_I (détecte le capteur absent), redémarrage du capteur après une erreur.
6. **Tests** : `outils/tests/test_<module>.c` (avec `assert`, `main` qui renvoie 0 si tout passe).
   Première ligne utile : `// SOURCES: <module>.c outils/tests/bus_simule.c` (fichiers à compiler avec).
   Seuls les modules testables sans le Pico sont testés (aujourd'hui : `capteur`).

## Tests automatiques
- `outils/scripts/verifier.sh` : compile le firmware, compile et lance les tests C sur le Mac (`cc`),
  vérifie les scripts Python, signale les fonctions de plus de 30 lignes et les fichiers de plus de 100 lignes.
  La CI GitHub (`.github/workflows/ci.yml`) lance **le même script** : une seule logique de vérification.
- **Hook** (`.claude/settings.json`, PostToolUse sur Edit|Write) : lancé après chaque modification
  d'un `.c`, `.h`, `.py` ou `CMakeLists.txt`. Échec de compilation ou de test → code 2, l'erreur est
  renvoyée à Claude pour correction. Règles des 30 lignes par fonction et 100 lignes par fichier → simple avertissement.
- Lancement manuel : `./APP/outils/scripts/verifier.sh </dev/null`.
- **Hook git** (`.claude/hooks/verif_git.sh`, PreToolUse sur Bash) : bloque commit / push / merge hors règles
  (commit sur main/develop, merge vers main hors develop, merge vers develop hors branche de travail, push forcé sur main/develop, `git add` dans la même commande, > 4 fichiers, format du message, emoji, jeton GitHub).

## Pièges
- Les câbles STEMMA QT disponibles ont un connecteur blanc aux deux bouts : inutilisables sur le Pico.
- Breadboard : les 5 trous d'une même ligne sont reliés. Deux fils dans la même ligne = court-circuit (2026-10-01 : Pico qui chauffe, LED capteur éteinte ; Pico intact après coup). Pour le test, brancher capteur → Pico en direct, sans breadboard.
- RP2350 erratum E9 : une entrée avec pull-down peut rester bloquée à 1 → ne pas l'utiliser pour détecter un fil débranché.
- Header du breakout LSM6DSOX non soudé (juste enfiché) → contacts intermittents, bus I2C qui décroche. Le souder.
- Câblage : vérifier les noms imprimés sous le Pico (GP4/GP5, 3V3, GND). Les 4 fils ne sont pas sur la même rangée : SDA/SCL côté GP0, 3V3/GND côté VBUS. Sur le breakout, utiliser SDA/SCL et non SDX/SCX (bus auxiliaire).
- Critère du test de perturbations sur l'écart-type **par axe** (x, y, z), pas sur la norme : un bruit perpendiculaire à la gravité est quasi invisible dans |a| (effet du 2e ordre).
- `perturbations.py capture` et le Serial Monitor VS Code ne peuvent pas ouvrir le port en même temps.
- Le port série du Pico peut changer de nom (`usbmodem101`, `usbmodem1101`…) : toujours le chercher par `/dev/cu.usbmodem*`.
- Le moniteur série VS Code ne se reconnecte pas seul après un débranchement : tout message envoyé avant l'ouverture du port est perdu. D'où la bannière ré-affichée à chaque connexion (`stdio_usb_connected()`).
- Ne pas déplacer/renommer le dossier `APP/outils/build/` : CMake y garde des chemins absolus → `rm -rf APP/outils/build` et reconfigurer (le script le fait seul s'il manque).
- macOS : `cp` du .uf2 vers /Volumes/RP2350 peut bloquer (processus en état U). Préférer le glisser-déposer Finder.
