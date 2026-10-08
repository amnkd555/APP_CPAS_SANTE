#ifndef CAPTEUR_H
#define CAPTEUR_H

#include <stdbool.h>

bool capteur_demarrer(void);

bool capteur_lire(float acceleration_g[3]);

#endif
