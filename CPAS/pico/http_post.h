#ifndef HTTP_POST_H
#define HTTP_POST_H

// Envoie texte (taille caracteres) en POST a http://SERVEUR_IP:SERVEUR_PORT/SERVEUR_CHEMIN
// et attend la fin de la reponse du serveur.
void http_post(const char *texte, int taille);

#endif
