# Protocole : test du capteur près du robot UR3

## But

Trouver la plus petite distance entre le capteur et un moteur du robot où le moteur ne gêne pas la mesure.
Cette distance donne l'épaisseur du boîtier.

## Idée du test

Même posé sans bouger, le capteur tremble un peu : c'est le **bruit**.
Si le moteur gêne le capteur, le bruit augmente quand le robot est allumé.
On compare donc le bruit **robot éteint** et **robot allumé**, à plusieurs distances.

## Matériel

- Pico + capteur LSM6DSOX soudé, câble USB, Mac avec le firmware `mesure_capteur`.
- Plaques imprimées en 3D : 0 (capteur contre le capot), 5, 10, 20 et 40 mm. Elles s'empilent.
- Ruban de masquage (fixer la plaque sur le capot et tenir le câble).
- Téléphone pour les photos.

## Préparation (une seule fois)

1. Choisir **une** articulation du robot et la garder pour tout le test.
2. Fermer le moniteur série de VS Code (sinon le script ne peut pas lire le Pico).
3. Toujours le même état du robot quand il est allumé : à l'arrêt, ou le même programme de mouvement. Le noter.

## Mesures

Pour chaque distance (0, 5, 10, 20, 40 mm) :

1. Fixer le capteur sur la plaque, à plat, toujours dans le même sens. Coller la plaque contre le capot.
2. Scotcher le câble pour qu'il ne tire pas sur le capteur. Prendre une photo.
3. Robot **éteint**, ne rien toucher :
   ```sh
   python3 APP/outils/scripts/perturbations.py capture d010_off_r1 --duree 30
   ```
4. Allumer le robot, sans toucher au montage :
   ```sh
   python3 APP/outils/scripts/perturbations.py capture d010_on_r1 --duree 30
   ```
5. Démonter puis remonter le capteur, et refaire 3 et 4 avec `r2`, puis `r3`.

Le nom du fichier dit tout : `d010` = 10 mm, `off` = éteint, `on` = allumé, `r1` = 1re fois.
Pour 0 mm : `d000`. Pour 5 mm : `d005`.

À chaque capture, le script demande un commentaire : y écrire l'articulation et l'état du robot.

## Fixer le seuil (avant d'analyser)

Deux mesures robot éteint n'ont jamais tout à fait le même bruit : il varie un peu tout seul.
Le robot ne gêne que s'il ajoute **plus** de bruit que cette variation normale.

1. Regarder les écarts-types (le bruit) des fichiers `off` r1, r2, r3 d'une même distance.
   Le script les affiche à la fin de chaque capture.
2. Calculer pour chaque axe : plus grand bruit / plus petit bruit. Exemple : 0,62 / 0,58 = 1,07.
3. Le seuil est le plus grand de ces rapports, arrondi au-dessus. Exemple : 1,07 donne 1,10.

## Analyse

```sh
python3 APP/outils/scripts/perturbations.py analyse --seuil 1.10
```

Remplacer `1.10` par le seuil trouvé. Le script affiche un tableau : une ligne par distance,
`OK` si le robot allumé ne gêne pas, `PERTURBE` sinon, puis la distance minimale.

## Résultat

- Distance minimale = celle donnée par le script + environ 1,6 mm (épaisseur de la carte du capteur).
- Garder pour la fiche de compétences : le tableau du script, les photos du montage
  et l'épaisseur choisie pour le boîtier.
