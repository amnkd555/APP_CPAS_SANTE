#ifndef CSV_DONNEES_H
#define CSV_DONNEES_H

// Contenu du CSV, ajoute au firmware par CMake (csv_vers_c.cmake).
// "const" : le tableau reste en flash, il n'occupe pas de RAM.
extern const char csv_donnees[];
extern const unsigned int csv_taille;   // nombre d'octets, sans le 0 final

#endif
