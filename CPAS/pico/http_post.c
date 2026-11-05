// Envoie un texte au serveur avec une requete HTTP POST, en passant par TCP (bibliotheque lwIP).
// lwIP marche avec des "callbacks" : on lui donne des fonctions, il les appelle quand
// la connexion est etablie (quand_connecte) et quand la reponse arrive (quand_recu).
#include <stdio.h>
#include <string.h>
#include "pico/cyw43_arch.h"
#include "lwip/tcp.h"
#include "config.h"
#include "http_post.h"

static char entete[200];         // l'en-tete HTTP
static const char *corps;        // le texte a envoyer (les lignes du CSV)
static int taille_corps;
static volatile bool termine;    // passe a true quand le serveur a fini de repondre

// Connexion etablie : on envoie l'en-tete puis le corps
static err_t quand_connecte(void *arg, struct tcp_pcb *pcb, err_t err)
{
    tcp_write(pcb, entete, strlen(entete), TCP_WRITE_FLAG_COPY);
    tcp_write(pcb, corps, taille_corps, TCP_WRITE_FLAG_COPY);
    tcp_output(pcb);
    return ERR_OK;
}

// Reponse du serveur. p == NULL : le serveur a ferme la connexion, c'est fini.
static err_t quand_recu(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    if (p == NULL) {
        tcp_close(pcb);
        termine = true;
        return ERR_OK;
    }
    tcp_recved(pcb, p->tot_len);   // dit a lwIP qu'on a lu les donnees
    pbuf_free(p);                  // libere la memoire de la reponse
    return ERR_OK;
}

void http_post(const char *texte, int taille)
{
    snprintf(entete, sizeof entete,
             "POST %s HTTP/1.0\r\nHost: %s\r\nContent-Type: text/csv\r\nContent-Length: %d\r\n\r\n",
             SERVEUR_CHEMIN, SERVEUR_IP, taille);
    corps = texte;
    taille_corps = taille;
    termine = false;

    ip_addr_t adresse;
    ipaddr_aton(SERVEUR_IP, &adresse);   // "192.168.0.10" -> adresse IP

    // begin/end : on prend la main sur lwIP, qui tourne en arriere-plan
    cyw43_arch_lwip_begin();
    struct tcp_pcb *pcb = tcp_new();
    tcp_recv(pcb, quand_recu);
    tcp_connect(pcb, &adresse, SERVEUR_PORT, quand_connecte);
    cyw43_arch_lwip_end();

    // On attend que le serveur ait repondu
    while (!termine) {
        sleep_ms(10);
    }
}
