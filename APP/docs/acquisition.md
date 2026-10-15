# acquisition.c

Fait une mesure et l'envoie au Mac. `main.c` l'appelle sans arrêt.

## À chaque tour

1. Si le Mac vient de se connecter, envoie l'en-tête : la date du programme, son nom, puis le nom des colonnes.
2. Si le capteur n'est pas encore prêt, le démarre.
   - Capteur absent : la LED clignote 5 fois vite, puis on réessaie.
   - Capteur trouvé : la LED reste allumée.
3. Lit le capteur.
   - Lecture réussie : envoie une ligne de chiffres.
   - Lecture ratée : envoie `# erreur I2C`. Le capteur sera redémarré au tour suivant.

## Une ligne de mesure

Exemple : `3615,0.01049,-0.03483,0.99997,1.00063,0.009,0.219,-0.674`

| Colonne | Ce que c'est |
|---|---|
| `timestamp_ms` | temps depuis l'allumage du Pico, en millisecondes |
| `ax_g`, `ay_g`, `az_g` | accélération sur chaque axe, en g (1 g = la pesanteur) |
| `mag_g` | accélération totale. Vaut environ 1 quand le capteur ne bouge pas |
| `gx_dps`, `gy_dps`, `gz_dps` | vitesse de rotation autour de chaque axe, en degrés par seconde. Vaut environ 0 quand le capteur ne bouge pas |

Les lignes qui commencent par `#` sont des messages, pas des mesures.
