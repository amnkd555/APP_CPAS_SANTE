cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico2_w CACHE STRING "Carte cible")
include(pico_sdk_import.cmake)
project(cpas_envoi C CXX ASM)
pico_sdk_init()

# Le CSV devient un tableau C (csv_donnees.c) a chaque compilation
set(CSV ${CMAKE_CURRENT_LIST_DIR}/donnees/reference_mesures_sedentarite.csv)
set(CSV_C ${CMAKE_CURRENT_BINARY_DIR}/csv_donnees.c)
add_custom_command(
    OUTPUT ${CSV_C}
    COMMAND ${CMAKE_COMMAND} -DENTREE=${CSV} -DSORTIE=${CSV_C} -P ${CMAKE_CURRENT_LIST_DIR}/csv_vers_c.cmake
    DEPENDS ${CSV} ${CMAKE_CURRENT_LIST_DIR}/csv_vers_c.cmake
)

add_executable(cpas_envoi main.c envoi.c http_post.c ${CSV_C})
target_include_directories(cpas_envoi PRIVATE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(cpas_envoi pico_stdlib pico_cyw43_arch_lwip_threadsafe_background)

pico_enable_stdio_usb(cpas_envoi 1)
pico_enable_stdio_uart(cpas_envoi 0)
pico_add_extra_outputs(cpas_envoi)
