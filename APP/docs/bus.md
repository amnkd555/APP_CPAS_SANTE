# bus.c

Gère le bus I2C0 : les deux fils qui relient le Pico au capteur.

- SDA (données) : GP4
- SCL (horloge) : GP5
- Vitesse : 400 kHz

## `bus_demarrer()`

Allume l'I2C0 et branche GP4/GP5 dessus.

## `bus_ecrire(adresse, registre, valeur)`

Envoie 2 octets au composant : le numéro du registre, puis la valeur à y écrire.

## `bus_lire(adresse, registre, destination, n)`

1. Envoie le numéro du registre à lire.
2. Lit `n` octets et les range dans `destination`.

## Délai maximum

Chaque échange est limité à 2 ms (`DELAI_MAX_US`). Si le capteur ne répond pas, la fonction renvoie `false` au lieu de bloquer le programme.
