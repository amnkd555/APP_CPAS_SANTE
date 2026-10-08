// Faux bus I2C pour les tests : remplace bus.c sur le Mac.
// Un seul composant, vu comme 256 registres, qui répond à toutes les adresses.
#include <stdbool.h>
#include <stdint.h>

#include "bus.h"

uint8_t registres[256];   // contenu des registres, rempli par le test
bool bus_muet = false;    // true = plus aucune réponse (fil coupé)

void bus_demarrer(void) {}

bool bus_ecrire(uint8_t adresse, uint8_t registre, uint8_t valeur) {
    (void)adresse;
    registres[registre] = valeur;
    return !bus_muet;
}

bool bus_lire(uint8_t adresse, uint8_t registre, uint8_t *destination, size_t n) {
    (void)adresse;
    for (size_t i = 0; i < n; i++) destination[i] = registres[(uint8_t)(registre + i)];
    return !bus_muet;
}
