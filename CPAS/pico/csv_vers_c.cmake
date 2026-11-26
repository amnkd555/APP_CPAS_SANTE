# Transforme les fichiers CSV de donnees/ en tableaux C constants, avec la liste de leurs noms.
# Appel (fait par CMakeLists.txt) : cmake "-DENTREES=a.csv;b.csv" -DSORTIE=csv_donnees.c -P csv_vers_c.cmake

set(texte "// Genere par csv_vers_c.cmake a partir de donnees/ : ne pas modifier a la main.\n#include \"csv_donnees.h\"\n\n")
set(noms "")
set(contenus "")
set(numero 0)

foreach(entree ${ENTREES})
    file(READ "${entree}" octets HEX)                                  # "2354..." : 2 chiffres hexa par octet
    string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," octets "${octets}")
    get_filename_component(nom "${entree}" NAME)                       # nom du fichier sans le dossier
    string(APPEND texte "static const char fichier_${numero}[] = {\n${octets}0x00};\n\n")
    string(APPEND noms "    \"${nom}\",\n")
    string(APPEND contenus "    fichier_${numero},\n")
    math(EXPR numero "${numero} + 1")
endforeach()

string(APPEND texte "const char *const csv_noms[] = {\n${noms}};\n\n")
string(APPEND texte "const char *const csv_contenus[] = {\n${contenus}};\n\n")
string(APPEND texte "const int csv_nombre = ${numero};\n")
file(WRITE "${SORTIE}" "${texte}")
