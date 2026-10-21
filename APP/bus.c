#include "bus.h"

#include "hardware/i2c.h"
#include "pico/stdlib.h"

#define BROCHE_SDA    4      // fil de données
#define BROCHE_SCL    5      // fil d'horloge
#define DELAI_MAX_US  2000   // un échange ne dure jamais plus de 2 ms (évite de rester bloqué)

// Allume l'I2C0 à 100 kHz et branche les broches SDA et SCL dessus.
void bus_demarrer(void) {
    i2c_init(i2c0, 100000);
    gpio_set_function(BROCHE_SDA, GPIO_FUNC_I2C);
    gpio_set_function(BROCHE_SCL, GPIO_FUNC_I2C);
}

// Écrit une valeur dans un registre du composant : envoie 2 octets (registre, valeur).
// Renvoie false si le composant ne répond pas.
bool bus_ecrire(uint8_t adresse, uint8_t registre, uint8_t valeur) {
    uint8_t trame[2] = {registre, valeur};
    return i2c_write_timeout_us(i2c0, adresse, trame, 2, false, DELAI_MAX_US) == 2;
}

// Lit n octets à partir d'un registre : on dit quel registre, puis on lit la réponse.
// Renvoie false si le composant ne répond pas.
bool bus_lire(uint8_t adresse, uint8_t registre, uint8_t *destination, size_t n) {
    if (i2c_write_timeout_us(i2c0, adresse, &registre, 1, true, DELAI_MAX_US) != 1) return false;
    return i2c_read_timeout_us(i2c0, adresse, destination, n, false, DELAI_MAX_US) == (int)n;
}
