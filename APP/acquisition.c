#include "acquisition.h"

#include <inttypes.h>
#include <math.h>
#include <stdio.h>

#include "capteur.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

static bool capteur_pret = false;
static bool port_ouvert = false;

static void led(bool allumee) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, allumee);
}

static void envoyer_entete_si_ouverture(void) {
    bool ouvert = stdio_usb_connected();
    if (ouvert && !port_ouvert) {
        printf("# Anti-Sedentarite - LSM6DSOX (+/-2 g, 104 Hz)\n");
        printf("timestamp_ms,ax_g,ay_g,az_g,mag_g\n");
    }
    port_ouvert = ouvert;
}

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

static void envoyer_mesure(const float a[3]) {
    float total = sqrtf(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
    printf("%" PRIu32 ",%.5f,%.5f,%.5f,%.5f\n", to_ms_since_boot(get_absolute_time()),
           (double)a[0], (double)a[1], (double)a[2], (double)total);
}

void acquisition_cycle(void) {
    envoyer_entete_si_ouverture();
    if (!capteur_pret) demarrer_capteur();
    float acceleration_g[3];
    if (capteur_lire(acceleration_g)) {
        envoyer_mesure(acceleration_g);
    } else {
        printf("# erreur I2C\n");
        capteur_pret = false;
    }
}
