#ifndef CONFIG_H
#define CONFIG_H

// Reglages du Pico. Copier ce fichier en config.h, puis remplir les vraies valeurs.
// config.h n'est pas mis sur Git (voir .gitignore) : il contient le mot de passe Wi-Fi.

#define WIFI_SSID          "wifirpi"
#define WIFI_MOT_DE_PASSE  "A_REMPLACER"

#define SERVEUR_IP         "192.168.0.10"          // IP du PC qui fait tourner XAMPP (ipconfig)
#define SERVEUR_PORT       80
#define SERVEUR_CHEMIN     "/cpas/reception.php"

// Colonnes a envoyer, separees par des virgules, sans espace autour des virgules.
// Les noms doivent etre ecrits exactement comme dans la ligne de titres du CSV.
// Colonnes disponibles : timestamp_ms,ax_g,ay_g,az_g,mag_g,gx_dps,gy_dps,gz_dps
#define COLONNES           "timestamp_ms,ax_g,ay_g,az_g,mag_g,gx_dps,gy_dps,gz_dps"

#define LIGNES_PAR_PAQUET  50                      // mesures envoyees par requete POST

#endif
