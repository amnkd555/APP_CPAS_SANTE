# APP — Dispositif anti-sédentarité

Boîtier portable (Raspberry Pi Pico 2 W + capteur LSM6DSOX) qui mesure l'activité physique (score MET).

## Le code

Tout le code du Pico est dans ce dossier : `main.c`, `acquisition.c`, `capteur.c`, `bus.c` (+ leurs `.h`).
Le dossier `outils/` contient la fabrication, les tests et les scripts : pas besoin de l'ouvrir pour lire le code.
Le dossier `docs/` explique chaque fichier `.c`.

## Démarrage rapide

Depuis la racine du dépôt :

```sh
export PICO_SDK_PATH=~/.pico-sdk/sdk
cmake -S APP/outils -B APP/outils/build -G Ninja && cmake --build APP/outils/build   # compile
./APP/outils/scripts/verifier.sh </dev/null                                         # tests
```
