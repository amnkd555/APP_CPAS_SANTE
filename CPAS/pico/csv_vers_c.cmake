# Transforme un fichier en tableau C constant.
# Appel (fait par CMakeLists.txt) : cmake -DENTREE=fichier.csv -DSORTIE=csv_donnees.c -P csv_vers_c.cmake

file(READ "${ENTREE}" octets HEX)                 # "2354..." : 2 chiffres hexa par octet
string(LENGTH "${octets}" longueur)
math(EXPR taille "${longueur} / 2")
string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," octets "${octets}")

file(WRITE "${SORTIE}"
    "// Genere par csv_vers_c.cmake a partir de donnees/ : ne pas modifier a la main.\n"
    "#include \"csv_donnees.h\"\n\n"
    "const unsigned int csv_taille = ${taille};\n"
    "const char csv_donnees[] = {\n${octets}0x00};\n")
