#include "capteur.h"

#include <stdint.h>
#include <stdio.h>

#include "bus.h"

#define ADRESSE        0x6A
#define REG_WHO_AM_I   0x0F
#define REG_CTRL1_XL   0x10
#define REG_STATUS     0x1E
#define REG_OUTX_L_A   0x28
#define G_PAR_UNITE    0.000061f

bool capteur_demarrer(void) {
    uint8_t identite = 0;
    if (!bus_lire(ADRESSE, REG_WHO_AM_I, &identite, 1) || identite != 0x6C) {
        printf("# ERREUR : LSM6DSOX introuvable -> verifier le cablage\n");
        return false;
    }
    return bus_ecrire(ADRESSE, REG_CTRL1_XL, 0x40);
}

bool capteur_lire(float acceleration_g[3]) {
    uint8_t status = 0;
    while (!(status & 0x01)) {
        if (!bus_lire(ADRESSE, REG_STATUS, &status, 1)) return false;
    }
    uint8_t brut[6];
    if (!bus_lire(ADRESSE, REG_OUTX_L_A, brut, 6)) return false;
    for (int axe = 0; axe < 3; axe++) {
        int16_t valeur = (int16_t)(brut[2 * axe + 1] << 8 | brut[2 * axe]);
        acceleration_g[axe] = valeur * G_PAR_UNITE;
    }
    return true;
}
