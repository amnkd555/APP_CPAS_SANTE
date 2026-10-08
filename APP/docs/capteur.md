# capteur.c

Parle au capteur LSM6DSOX (accéléromètre) à l'adresse I2C `0x6A`.

## `capteur_demarrer()`

1. Lit le registre WHO_AM_I (`0x0F`). Le LSM6DSOX répond toujours `0x6C`.
   - Pas de réponse ou mauvaise valeur : affiche `# ERREUR : LSM6DSOX introuvable` et renvoie `false`.
2. Écrit `0x40` dans `CTRL1_XL` (`0x10`) : mesure à 104 Hz, plage ±2 g.
3. Renvoie `true` si tout s'est bien passé.

## `capteur_lire(acceleration_g)`

1. Attend qu'une nouvelle mesure soit prête (bit 0 du registre `STATUS`, `0x1E`).
2. Lit 6 octets à partir de `OUTX_L_A` (`0x28`) : x, y, z, 2 octets chacun.
3. Recolle chaque paire d'octets en un nombre signé (octet bas + octet haut).
4. Multiplie par `0.000061` pour obtenir des g (valeur donnée par la fiche technique pour ±2 g).
5. Renvoie `false` dès qu'un échange I2C échoue.
