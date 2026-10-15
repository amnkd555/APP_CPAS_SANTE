#include "capteur.h"

#include <stdint.h>
#include <stdio.h>

#include "bus.h"

#define ADRESSE        0x6A       // adresse I2C du capteur
#define REG_WHO_AM_I   0x0F       // registre qui contient l'identité du capteur
#define REG_CTRL1_XL   0x10       // registre de réglage de l'accéléromètre
#define REG_CTRL2_G    0x11       // registre de réglage du gyroscope
#define REG_STATUS     0x1E       // registre qui dit si une mesure est prête
#define REG_OUTX_L_G   0x22       // début des 12 octets de mesure (gyro x, y, z puis accéléro x, y, z)
#define G_PAR_UNITE    0.000061f  // 1 graduation de l'accéléromètre = 0,000061 g (plage +/-2 g)
#define DPS_PAR_UNITE  0.00875f   // 1 graduation du gyroscope = 0,00875 degré/s (plage +/-250 dps)

// Vérifie que le capteur répond, puis règle l'accéléromètre (+/-2 g)
// et le gyroscope (+/-250 dps), tous deux à 104 mesures par seconde.
bool capteur_demarrer(void) {
    uint8_t identite = 0;
    // le LSM6DSOX répond toujours 0x6C ; sinon il est absent ou mal câblé
    if (!bus_lire(ADRESSE, REG_WHO_AM_I, &identite, 1) || identite != 0x6C) {
        printf("# ERREUR : LSM6DSOX introuvable -> verifier le cablage\n");
        return false;
    }
    return bus_ecrire(ADRESSE, REG_CTRL1_XL, 0x40) && bus_ecrire(ADRESSE, REG_CTRL2_G, 0x40);
}

// Recolle 2 octets (octet bas puis octet haut) en un nombre signé, puis convertit en unité réelle.
static void convertir(const uint8_t brut[6], float facteur, float resultat[3]) {
    for (int axe = 0; axe < 3; axe++) {
        int16_t valeur = (int16_t)(brut[2 * axe + 1] << 8 | brut[2 * axe]);
        resultat[axe] = valeur * facteur;
    }
}

// Lit l'accélération (en g) et la rotation (en degrés par seconde) des 3 axes.
// Renvoie false si un échange I2C échoue.
bool capteur_lire(float acceleration_g[3], float rotation_dps[3]) {
    uint8_t status = 0;
    // on attend que les 2 mesures soient prêtes (bit 0 : accéléro, bit 1 : gyro)
    while ((status & 0x03) != 0x03) {
        if (!bus_lire(ADRESSE, REG_STATUS, &status, 1)) return false;
    }
    uint8_t brut[12];   // 6 octets de gyroscope, puis 6 octets d'accéléromètre
    if (!bus_lire(ADRESSE, REG_OUTX_L_G, brut, 12)) return false;
    convertir(brut, DPS_PAR_UNITE, rotation_dps);
    convertir(brut + 6, G_PAR_UNITE, acceleration_g);
    return true;
}
