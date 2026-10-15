#include "acquisition.h"

#include <inttypes.h>
#include <math.h>
#include <stdio.h>

#include "capteur.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

static bool capteur_pret = false;   // le capteur a-t-il bien démarré ?
static bool port_ouvert = false;    // le Mac est-il connecté au port USB ?

// Allume ou éteint la LED de la carte.
static void led(bool allumee) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, allumee);
}

// Envoie l'en-tête du CSV une fois, au moment où le Mac ouvre le port.
// La ligne "# build" donne la date de compilation : elle prouve quel firmware est flashé.
static void envoyer_entete_si_ouverture(void) {
    bool ouvert = stdio_usb_connected();
    if (ouvert && !port_ouvert) {
        printf("# build " __DATE__ " " __TIME__ "\n");
        printf("# Anti-Sedentarite - LSM6DSOX (+/-2 g, +/-250 dps, 104 Hz)\n");
        printf("timestamp_ms,ax_g,ay_g,az_g,mag_g,gx_dps,gy_dps,gz_dps\n");
    }
    port_ouvert = ouvert;
}

// Démarre le capteur. S'il est absent : 5 flashs rapides, puis on réessaie.
// Quand il répond : LED fixe.
static void demarrer_capteur(void) {
    while (!capteur_demarrer()) {
        for (int i = 0; i < 5; i++) {
            led(true);
            sleep_ms(100);
            led(false);
            sleep_ms(100);
        }
    }
    led(true);
    capteur_pret = true;
}

// Calcule la magnitude puis envoie une ligne CSV :
// temps depuis l'allumage (ms), ax, ay, az, magnitude, puis gx, gy, gz (rotation).
static void envoyer_mesure(const float a[3], const float g[3]) {
    float total = sqrtf(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);   // magnitude
    printf("%" PRIu32 ",%.5f,%.5f,%.5f,%.5f,%.3f,%.3f,%.3f\n", to_ms_since_boot(get_absolute_time()),
           (double)a[0], (double)a[1], (double)a[2], (double)total,
           (double)g[0], (double)g[1], (double)g[2]);
}

// Une mesure complète : en-tête si besoin, démarrage du capteur si besoin,
// lecture, envoi. Si la lecture échoue, le capteur sera redémarré au tour suivant.
void acquisition_cycle(void) {
    envoyer_entete_si_ouverture();
    if (!capteur_pret) demarrer_capteur();
    float acceleration_g[3], rotation_dps[3];
    if (capteur_lire(acceleration_g, rotation_dps)) {
        envoyer_mesure(acceleration_g, rotation_dps);
    } else {
        printf("# erreur I2C\n");
        capteur_pret = false;
    }
}
