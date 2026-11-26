# Comprendre les mesures du LSM6DSOX

Le LSM6DSOX contient deux capteurs : un **accéléromètre** (3 axes) et un **gyroscope** (3 axes).
Réglages du projet : accéléromètre ±2 g, gyroscope ±250 degrés/s, 104 mesures par seconde.

## Une ligne du CSV

```
19337,-0.07174,-0.05075,0.99869,1.00255,-1.986,-0.245,-2.170
```

| Colonne | Unité | Ce que c'est |
|---|---|---|
| timestamp_ms | ms | temps depuis l'allumage du Pico |
| ax_ms2, ay_ms2, az_ms2 | m/s² | accélération sur chaque axe (avant le 1er novembre 2026 : ax_g... en g) |
| mag_ms2 | m/s² | accélération totale : racine(ax² + ay² + az²) |
| gx_dps, gy_dps, gz_dps | degrés/s | vitesse de rotation autour de chaque axe |

Les axes x, y, z sont dessinés sur la carte du capteur.

## Accéléromètre (ax, ay, az)

**1 g = 9,81 m/s²**, l'accélération de la pesanteur.

L'accéléromètre mesure la pesanteur en plus du mouvement. Au repos, il indique donc 1 g sur l'axe vertical, pas 0.

| Situation | ax | ay | az | mag |
|---|---|---|---|---|
| À plat, immobile | 0 | 0 | 1 | 1 |
| À l'envers, immobile | 0 | 0 | -1 | 1 |
| Sur la tranche, immobile | 1 ou -1 (ou ay) | | 0 | 1 |
| En mouvement | varie | varie | varie | s'écarte de 1 |
| Chute libre | 0 | 0 | 0 | 0 |

- **Immobile** : mag = 1 g, quelle que soit l'orientation. La répartition sur les axes donne l'inclinaison.
- **Mouvement** : mag s'écarte de 1 g. Plus l'écart est grand, plus le mouvement est fort. C'est la base du score MET.
- Plage ±2 g : au-delà de 2 g sur un axe, la valeur reste bloquée à 2 (choc trop fort).

## Gyroscope (gx, gy, gz)

Il mesure la **vitesse de rotation**, pas l'angle. Exemple : gz = 90 signifie que le capteur tourne d'un quart de tour par seconde autour de z.

- **Immobile** : proche de 0. Quelques dixièmes de degré/s, parfois un peu plus, sont normaux (décalage du capteur).
- **Rotation** : la valeur grandit ; son signe indique le sens de rotation.
- Plage ±250 degrés/s : une rotation plus rapide est bloquée à 250.

## Défauts normaux du capteur

D'après la documentation du fabricant (ST) :

| Défaut | Accéléromètre | Gyroscope |
|---|---|---|
| Décalage au repos (valeur typique) | ±0,02 g | ±1 degré/s |
| Plus petite variation mesurable | 0,000061 g | 0,00875 degré/s |

- Le **décalage** explique que mag vaille 1,005 au lieu de 1,000, et que gz vaille -0,6 au repos.
- Le **bruit** fait trembler chaque mesure un peu. La moyenne l'efface, l'écart-type le mesure.

## Interpréter une série de mesures

| Je vois | Cela veut dire |
|---|---|
| mag ≈ 1 et gyro ≈ 0, stables | capteur immobile |
| ax ou ay ≠ 0 au repos | capteur penché |
| mag qui varie, gyro ≠ 0 | capteur qui bouge |
| écart-type plus grand que la référence (0,0006 g) | perturbation ou vibration |
| moyenne décalée par rapport à la référence | quelque chose fausse la mesure |

Valeurs de référence (capteur à plat, immobile) : voir `reference_plat.md`.

## Voir les mesures en direct

```sh
screen /dev/cu.usbmodem* 115200
```

Pour arrêter : **Ctrl+A**, puis **K**, puis **y**.
