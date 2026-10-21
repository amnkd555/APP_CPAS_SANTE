#include "acquisition.h"
#include "bus.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

// Point de départ du programme.
int main(void) {
    stdio_init_all();    // active l'USB pour envoyer du texte au Mac
    cyw43_arch_init();   // démarre la puce Wi-Fi, qui pilote la LED de la carte
    bus_demarrer();      // prépare le bus I2C qui parle au capteur
    while (true) {
        acquisition_cycle();   // une mesure à chaque tour, pour toujours
    }
}
