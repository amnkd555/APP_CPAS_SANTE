#ifndef ENVOI_H
#define ENVOI_H

// Envoie au serveur les colonnes choisies du fichier nom, pris dans pico/donnees/
// (ex. envoyer_csv("reference_mesures_sedentarite.csv")).
// Chaque appel cree un nouveau fichier sur le serveur. Le Wi-Fi doit deja etre connecte.
void envoyer_csv(const char *nom);

#endif
