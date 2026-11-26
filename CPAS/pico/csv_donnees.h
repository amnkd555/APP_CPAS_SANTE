#ifndef CSV_DONNEES_H
#define CSV_DONNEES_H

// Les fichiers CSV du dossier donnees/, ajoutes au firmware par CMake (csv_vers_c.cmake).
// "const" : les tableaux restent en flash, ils n'occupent pas de RAM.
extern const char *const csv_noms[];       // nom de chaque fichier, ex. "reference_mesures_sedentarite.csv"
extern const char *const csv_contenus[];   // texte de chaque fichier, dans le meme ordre
extern const int csv_nombre;               // nombre de fichiers

#endif
