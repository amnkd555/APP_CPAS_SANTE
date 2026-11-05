// Programme principal : connexion au Wi-Fi, puis envoi du CSV au serveur.
// LED allumee = envoi termine.
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "config.h"
#include "envoi.h"

int main(void)
{
    stdio_init_all();
    sleep_ms(5000);                                     // le temps d'ouvrir le moniteur serie

    cyw43_arch_init_with_country(CYW43_COUNTRY_FRANCE); // allume la puce Wi-Fi
    cyw43_arch_enable_sta_mode();                       // mode "client" : on se connecte a une borne
    cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_MOT_DE_PASSE, CYW43_AUTH_WPA2_AES_PSK, 30000);
    printf("Wi-Fi connecte\n");

    envoyer_csv();
    printf("Envoi termine\n");

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);      // allume la LED
    while (true) {
        sleep_ms(1000);
    }
}
