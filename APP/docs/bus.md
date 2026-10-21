# bus.c

Gère les deux fils qui relient le Pico au capteur et à l'horloge :
- GP4 : les données ;
- GP5 : le rythme des échanges.

Vitesse : 100 000 bits par seconde. C'est lent, mais plus fiable, et largement suffisant.

## Les trois fonctions

| Fonction | Ce qu'elle fait |
|---|---|
| `bus_demarrer()` | prépare les deux fils |
| `bus_ecrire()` | écrit un réglage dans un composant |
| `bus_lire()` | lit une ou plusieurs valeurs dans un composant |

Chaque échange dure au plus 2 ms. Si le composant ne répond pas, la fonction renvoie `false` au lieu de bloquer le programme.
