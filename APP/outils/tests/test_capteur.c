// SOURCES: capteur.c outils/tests/bus_simule.c
// Tests du capteur avec un faux bus I2C
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "capteur.h"

extern uint8_t registres[256];
extern bool bus_muet;

static void test_demarrage(void) {
    memset(registres, 0, sizeof registres);
    bus_muet = false;
    assert(!capteur_demarrer());   // identité fausse (0x00)
    registres[0x0F] = 0x6C;
    assert(capteur_demarrer());
    assert(registres[0x10] == 0x40);   // 104 Hz, ±2 g
}

static void test_lecture(void) {
    // X = 0, Y = -1 unité, Z = 0x4009 ≈ +1 g ; mesure prête
    uint8_t brut[6] = {0x00, 0x00, 0xFF, 0xFF, 0x09, 0x40};
    memcpy(&registres[0x28], brut, 6);
    registres[0x1E] = 0x01;
    float a[3];
    assert(capteur_lire(a));
    assert(a[0] == 0.0f);
    assert(fabsf(a[1] + 0.000061f) < 1e-9f);
    assert(fabsf(a[2] - 1.0f) < 1e-3f);
}

static void test_bus_muet(void) {
    bus_muet = true;
    float a[3];
    assert(!capteur_lire(a));
    assert(!capteur_demarrer());
}

int main(void) {
    test_demarrage();
    test_lecture();
    test_bus_muet();
    printf("test_capteur : OK\n");
    return 0;
}
