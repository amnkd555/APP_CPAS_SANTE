// Envoie le CSV (integre au firmware) au serveur, par paquets de LIGNES_PAR_PAQUET lignes.
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "csv_donnees.h"
#include "http_post.h"
#include "envoi.h"

static char paquet[LIGNES_PAR_PAQUET * 100];   // le texte du paquet en cours

void envoyer_csv(void)
{
    int taille = 0;                     // nombre de caracteres dans le paquet
    int lignes = 0;                     // nombre de lignes dans le paquet
    const char *ligne = csv_donnees;    // debut de la ligne lue

    while (*ligne != '\0') {
        const char *fin = strchr(ligne, '\n');   // fin de la ligne
        int n = fin - ligne + 1;                 // longueur de la ligne, '\n' compris

        // Une mesure commence par un chiffre (on saute les commentaires "#" et l'en-tete)
        if (ligne[0] >= '0' && ligne[0] <= '9') {
            memcpy(paquet + taille, ligne, n);
            taille += n;
            lignes++;
        }

        // Paquet plein : on l'envoie et on recommence un paquet vide
        if (lignes == LIGNES_PAR_PAQUET) {
            http_post(paquet, taille);
            printf("Paquet envoye\n");
            taille = 0;
            lignes = 0;
        }

        ligne = fin + 1;   // ligne suivante
    }

    // Le dernier paquet (moins de LIGNES_PAR_PAQUET lignes)
    if (lignes > 0) {
        http_post(paquet, taille);
        printf("Paquet envoye\n");
    }
}
