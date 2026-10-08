#include "acquisition.h"
#include "bus.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

int main(void) {
    stdio_init_all();
    cyw43_arch_init();
    bus_demarrer();
    while (true) {
        acquisition_cycle();
    }
}
