cmake_minimum_required(VERSION 3.13)

# Carte par défaut : Pico 2 W (à fixer avant d'inclure le SDK)
set(PICO_BOARD pico2_w CACHE STRING "Type de carte Pico")
include(cmake/pico_sdk_import.cmake)

project(anti_sedentarite C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

# Programme : build/mesure_capteur.uf2, printf envoyé sur l'USB (pas sur l'UART GP0/GP1)
# Le code est à la racine du dépôt, un niveau au-dessus de ce dossier outils/
set(CODE ${CMAKE_CURRENT_SOURCE_DIR}/..)
add_executable(mesure_capteur ${CODE}/main.c ${CODE}/acquisition.c ${CODE}/capteur.c ${CODE}/bus.c)
target_link_libraries(mesure_capteur pico_stdlib hardware_i2c pico_cyw43_arch_none)
pico_enable_stdio_usb(mesure_capteur 1)
pico_enable_stdio_uart(mesure_capteur 0)
pico_add_extra_outputs(mesure_capteur)
