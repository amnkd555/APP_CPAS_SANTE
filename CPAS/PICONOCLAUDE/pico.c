#include <cstdio>
#include <cstring>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/netif.h"
#include "lwip/ip4_addr.h"
#include "lwip/apps/http_client.h"
#include "hardware/sync.h"
#include "hardware/adc.h"
#include "hardware/structs/ioqspi.h"
#include "hardware/structs/sio.h"

// Wifi de la borne D-link DAP-1360 stockée dans la salle A214
const char *ssid = "wifirpi";
const char *password = "MOT_DE_PASSE_WIFI";

// Equivalent de rp2.bootsel_button() (fonction reprise des exemples officiels pico-examples)
bool __no_inline_not_in_flash_func(bootsel_button)() {
    const uint CS_PIN_INDEX = 1;
    uint32_t flags = save_and_disable_interrupts();
    hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
                    GPIO_OVERRIDE_LOW << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
                    IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);
    for (volatile int i = 0; i < 1000; ++i);
#if PICO_RP2040
    bool appuye = !(sio_hw->gpio_hi_in & (1u << CS_PIN_INDEX));
#else
    bool appuye = !(sio_hw->gpio_hi_in & SIO_GPIO_HI_IN_QSPI_CSN_BITS);   // Pico 2 W (RP2350)
#endif
    hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
                    GPIO_OVERRIDE_NORMAL << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
                    IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);
    restore_interrupts(flags);
    return appuye;
}

// Equivalent de pico_led.on() / pico_led.off()
void led(bool allume) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, allume);
}

// Renvoie true si connecté, false si on a appuyé sur BOOTSEL (= sys.exit())
bool connect() {
    // Connect to WLAN
    cyw43_arch_enable_sta_mode();
    cyw43_arch_wifi_connect_async(ssid, password, CYW43_AUTH_WPA2_AES_PSK);
    while (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_UP) {
        if (bootsel_button()) {
            return false;
        }
        printf("Waiting for connection...\n");
        led(true);
        sleep_ms(500);
        led(false);
        sleep_ms(500);
    }
    const char *ip = ip4addr_ntoa(netif_ip4_addr(&cyw43_state.netif[CYW43_ITF_STA]));
    printf("Connected on %s\n", ip);
    led(true);
    return true;
}

// ----- Requête HTTP : équivalent de requests.get(URL) -----
const char *serveur = "192.168.1.10";   // adresse IP du serveur
const int   port    = 80;               // port HTTP par défaut
const char *chemin  = "/temperature.php"; // page PHP dans C:\xampp\htdocs
const char *nom_parametre = "temperature"; // nom envoyé au serveur : /?temperature=23.45

char contenu[2048];                     // response.content
int  taille_contenu = 0;
int  response_code = 0;                 // response.status_code
volatile bool requete_finie = false;
httpc_connection_t reglages = {};

// Appelée à chaque morceau de contenu reçu : on le stocke dans "contenu"
err_t recevoir(void *arg, struct altcp_pcb *conn, struct pbuf *p, err_t err) {
    if (p == NULL) return ERR_OK;
    for (struct pbuf *q = p; q != NULL; q = q->next) {
        int n = q->len;
        if (taille_contenu + n > (int)sizeof(contenu) - 1) n = sizeof(contenu) - 1 - taille_contenu;
        memcpy(contenu + taille_contenu, q->payload, n);
        taille_contenu += n;
    }
    contenu[taille_contenu] = '\0';
    altcp_recved(conn, p->tot_len);
    pbuf_free(p);
    return ERR_OK;
}

// Appelée quand la requête est terminée
void fin_requete(void *arg, httpc_result_t resultat, u32_t taille, u32_t code, err_t err) {
    response_code = code;
    requete_finie = true;
}

void requests_get(const char *page) {
    taille_contenu = 0;
    contenu[0] = '\0';
    response_code = 0;
    requete_finie = false;
    reglages.result_fn = fin_requete;

    ip_addr_t adresse;
    ipaddr_aton(serveur, &adresse);

    cyw43_arch_lwip_begin();
    err_t err = httpc_get_file(&adresse, port, page, &reglages, recevoir, NULL, NULL);
    cyw43_arch_lwip_end();

    if (err != ERR_OK) {
        printf("Request failed\n");
        return;
    }
    while (!requete_finie) {
        sleep_ms(10);
    }
}

// ----- Capteur de température interne : équivalent de pico_temp_sensor.temp -----
void init_temperature() {
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(ADC_TEMPERATURE_CHANNEL_NUM);
}

float lire_temperature() {
    const float conversion = 3.3f / (1 << 12);       // ADC 12 bits, 3,3 V
    float tension = adc_read() * conversion;
    return 27.0f - (tension - 0.706f) / 0.001721f;   // formule de la datasheet
}

int main() {
    stdio_init_all();
    cyw43_arch_init();
    init_temperature();

    if (!connect()) {
        cyw43_arch_deinit();
        return 0;
    }

    printf("Connected - press BOOTSEL to quit\n");
    while (true) {
        if (bootsel_button()) {
            led(false);
            printf("ByBye\n");
            cyw43_arch_deinit();
            return 0;
        }
        float temperature = lire_temperature();
        printf("Temperature: %.2f C\n", temperature);

        // Envoi au serveur : GET http://192.168.1.10/temperature.php?temperature=23.45
        char page[100];
        snprintf(page, sizeof(page), "%s?%s=%.2f", chemin, nom_parametre, temperature);
        requests_get(page);
        printf("Response code: %d\n", response_code);
        printf("Response content: %s\n", contenu);
        sleep_ms(2000);
    }
}