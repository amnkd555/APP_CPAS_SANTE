#ifndef BUS_H
#define BUS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void bus_demarrer(void);

bool bus_ecrire(uint8_t adresse, uint8_t registre, uint8_t valeur);

bool bus_lire(uint8_t adresse, uint8_t registre, uint8_t *destination, size_t n);

#endif
