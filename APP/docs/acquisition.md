# acquisition.c

Fait une mesure et l'envoie au Mac. Appelé en boucle par `main.c`.

## `acquisition_cycle()` (la seule fonction publique)

À chaque tour :

1. Si le Mac vient d'ouvrir le port série, envoie l'en-tête :
   ```
   # Anti-Sedentarite - LSM6DSOX (+/-2 g, 104 Hz)
   timestamp_ms,ax_g,ay_g,az_g,mag_g
   ```
2. Si le capteur n'est pas prêt, le démarre.
3. Lit l'accélération sur x, y, z.
   - Lecture OK : envoie une ligne CSV, par ex. `15230,0.01234,-0.00512,0.99871,0.99880`.
   - Lecture ratée : envoie `# erreur I2C` et note que le capteur devra être redémarré au tour suivant.

## Fonctions internes

| Fonction | Rôle |
|---|---|
| `led()` | allume ou éteint la LED |
| `envoyer_entete_si_ouverture()` | envoie l'en-tête une fois par ouverture du port |
| `demarrer_capteur()` | tant que le capteur ne répond pas : 5 flashs rapides puis on réessaie. Une fois OK : LED fixe |
| `envoyer_mesure()` | calcule la norme `mag_g = racine(ax² + ay² + az²)` et écrit la ligne CSV |

## Colonnes du CSV

| Colonne | Contenu |
|---|---|
| `timestamp_ms` | temps depuis l'allumage du Pico, en ms |
| `ax_g`, `ay_g`, `az_g` | accélération par axe, en g (1 g = gravité) |
| `mag_g` | norme de l'accélération. Vaut environ 1 si le capteur est immobile |
