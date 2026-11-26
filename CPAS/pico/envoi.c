// Envoie au serveur les colonnes choisies (COLONNES dans config.h) d'un CSV de donnees/,
// designe par son nom (les CSV sont integres au firmware a la compilation).
// Chaque paquet = la ligne de titres + LIGNES_PAR_PAQUET lignes de mesures.
// Le numero d'envoi (tire au hasard) dit au serveur quels paquets vont dans le meme fichier.
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "pico/rand.h"
#include "config.h"
#include "csv_donnees.h"
#include "http_post.h"
#include "envoi.h"

static char paquet[LIGNES_PAR_PAQUET * 200 + 400];   // le texte du paquet en cours (200 caracteres max par ligne)
static int taille;                    // nombre de caracteres dans le paquet
static int lignes;                    // nombre de mesures dans le paquet
static bool garder[20];               // garder[i] = true : la colonne i du CSV est envoyee
static const char *titres;            // la ligne de titres du CSV
static char chemin[100];              // "/cpas/reception.php?envoi=123456"

// true si nom fait partie de la liste COLONNES
static bool est_choisie(const char *nom)
{
    char cherche[40];
    snprintf(cherche, sizeof cherche, ",%s,", nom);
    return strstr("," COLONNES ",", cherche) != NULL;
}

// Lit la ligne de titres et note quelles colonnes sont a envoyer
static void choisir_colonnes(const char *ligne)
{
    char nom[32];
    int n = 0, colonne = 0;
    for (const char *p = ligne; ; p++) {
        if (*p == ',' || *p == '\r' || *p == '\n') {
            nom[n] = '\0';
            garder[colonne] = est_choisie(nom);
            colonne++;
            n = 0;
            if (*p != ',') return;
        } else {
            nom[n++] = *p;
        }
    }
}

// Ajoute au paquet la ligne, en ne gardant que les colonnes choisies
static void ajouter(const char *ligne)
{
    const char *debut = ligne;   // debut de la valeur en cours
    int colonne = 0;
    bool premiere = true;
    for (const char *p = ligne; ; p++) {
        if (*p == ',' || *p == '\r' || *p == '\n') {
            if (garder[colonne]) {
                if (!premiere) paquet[taille++] = ',';
                memcpy(paquet + taille, debut, p - debut);
                taille += p - debut;
                premiere = false;
            }
            colonne++;
            debut = p + 1;
            if (*p != ',') break;
        }
    }
    paquet[taille++] = '\n';
}

// Paquet vide, qui commence par la ligne de titres
static void nouveau_paquet(void)
{
    taille = 0;
    lignes = 0;
    ajouter(titres);
}

// Le texte du fichier qui porte ce nom, ou NULL s'il n'existe pas
static const char *trouver(const char *nom)
{
    for (int i = 0; i < csv_nombre; i++) {
        if (strcmp(csv_noms[i], nom) == 0) return csv_contenus[i];
    }
    return NULL;
}

// Les lignes de titres commencent par une lettre, les mesures par un chiffre ou un signe -
static bool est_lettre(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
static bool est_nombre(char c) { return (c >= '0' && c <= '9') || c == '-'; }

void envoyer_csv(const char *nom)
{
    const char *csv = trouver(nom);
    if (csv == NULL) {
        printf("Fichier %s introuvable dans donnees/\n", nom);
        return;
    }
    printf("Envoi de %s\n", nom);
    snprintf(chemin, sizeof chemin, "%s?envoi=%lu", SERVEUR_CHEMIN, (unsigned long)get_rand_32());
    titres = NULL;                       // on cherchera la ligne de titres de ce fichier
    lignes = 0;
    const char *ligne = csv;

    while (*ligne != '\0') {
        const char *fin = strchr(ligne, '\n');
        if (titres == NULL && est_lettre(ligne[0])) {          // la ligne de titres
            titres = ligne;
            choisir_colonnes(ligne);
            nouveau_paquet();
        } else if (titres != NULL && est_nombre(ligne[0])) {   // une mesure
            ajouter(ligne);
            lignes++;
        }
        if (lignes == LIGNES_PAR_PAQUET) {                     // paquet plein : on l'envoie
            http_post(chemin, paquet, taille);
            printf("Paquet envoye\n");
            nouveau_paquet();
        }
        ligne = fin + 1;
    }

    if (titres == NULL) {
        printf("Pas de ligne de titres dans %s : rien envoye\n", nom);
    } else if (lignes > 0) {                                   // le dernier paquet
        http_post(chemin, paquet, taille);
        printf("Paquet envoye\n");
    }
}
