# main.c

Point de départ du programme.

1. `stdio_init_all()` : active l'USB pour pouvoir envoyer du texte au Mac.
2. `cyw43_arch_init()` : démarre la puce Wi-Fi, qui pilote la LED de la carte.
3. `bus_demarrer()` : prépare le bus I2C qui parle au capteur.
4. Ensuite, répète `acquisition_cycle()` à l'infini.
