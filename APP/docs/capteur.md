# capteur.c

Parle au capteur LSM6DSOX. Il contient deux capteurs :
- l'**accéléromètre** : mesure les secousses et la pesanteur ;
- le **gyroscope** : mesure les rotations.

## `capteur_demarrer()`

1. Demande au capteur son identité. Le LSM6DSOX répond toujours `0x6C`.
   - Pas de réponse : affiche `# ERREUR : LSM6DSOX introuvable`.
2. Règle l'accéléromètre : 104 mesures par seconde, jusqu'à 2 g.
3. Règle le gyroscope : 104 mesures par seconde, jusqu'à 250 degrés par seconde.

## `capteur_lire()`

1. Attend que les deux mesures soient prêtes.
2. Les lit d'un seul coup.
3. Le capteur donne des nombres sans unité. On les multiplie pour obtenir :
   - des g pour l'accélération (× 0,000061) ;
   - des degrés par seconde pour la rotation (× 0,00875).

   Ces nombres viennent de la fiche du capteur.
4. Si un échange avec le capteur rate, renvoie `false`.
