# CPAS - envoi des mesures du Pico vers le serveur XAMPP

Le Pico 2 W envoie les colonnes choisies d'un fichier de mesures a un serveur XAMPP.
Le serveur les range dans un fichier CSV (un fichier par envoi) et les affiche
sur une page web avec un graphique par colonne.

```
CSV -> Pico (colonnes choisies) -> Wi-Fi -> reception.php -> data/<date>_envoi<n>.csv
                                                         -> table MySQL envois (liste des fichiers)
```

## Choisir les mesures envoyees

Dans `pico/config.h` :
```
#define COLONNES "timestamp_ms,ax_g,mag_g"
```
Noms separes par des virgules, sans espace. Le Pico envoie ces colonnes dans l'ordre du CSV.
Le serveur s'adapte tout seul : il recoit les noms des colonnes en tete de chaque paquet.
Si `timestamp_ms` est envoye, il sert d'axe du temps pour les graphiques.

Explication avec schemas : `docs/CPAS_Fonctionnement_Simple.pdf`.

Cette version est volontairement minimale : aucune verification d'erreur, aucun reessai.
Si quelque chose ne marche pas (Wi-Fi, serveur arrete, mauvaise IP), le Pico ne le signale pas.

## Contenu

| Fichier | Role |
|---|---|
| `pico/main.c` | connexion Wi-Fi, appel de `envoyer_csv()`, LED allumee a la fin |
| `pico/envoi.c` | garde les colonnes choisies et envoie le CSV par paquets de 50 lignes |
| `pico/http_post.c` | requete HTTP POST avec l'API TCP de lwIP |
| `pico/donnees/` | le CSV integre au firmware a la compilation (`csv_vers_c.cmake`) |
| `pico/config.h` | SSID, mot de passe Wi-Fi, IP du PC, colonnes a envoyer (modele : `config_exemple.h`) |
| `serveur/` | a copier dans `C:\xampp\htdocs\cpas\` |
| `serveur/creer_base.sql` | base `cpas_sante`, table `envois` = liste des fichiers (a importer dans phpMyAdmin) |
| `serveur/reception.php` | recoit un paquet et l'ajoute au fichier CSV de l'envoi (`data/`) |
| `serveur/index.php` | choix de l'envoi, un graphique par colonne, 20 dernieres mesures, lien vers le CSV |

## Demarrage rapide

1. `pico/config.h` : mettre le mot de passe Wi-Fi et l'IP du PC.
2. Compiler (sous WSL) : `./pico/compiler.sh` -> `~/build-cpas/cpas_envoi.uf2`.
3. Serveur : demarrer Apache et MySQL dans XAMPP, importer `serveur/creer_base.sql`
   (n'efface rien), copier le contenu de `serveur/` dans `C:\xampp\htdocs\cpas\`.
4. Tester le serveur sans le Pico (PowerShell, dans `docs/`) :
   ```
   curl.exe --data-binary "@paquet_test.csv" "http://localhost/cpas/reception.php?envoi=1"
   ```
   puis ouvrir `http://localhost/cpas/`.
5. Flasher : BOOTSEL + brancher, glisser `cpas_envoi.uf2` sur le lecteur `RP2350`.

## Format envoye par le Pico

```
POST /cpas/reception.php?envoi=3735928559 HTTP/1.0
Content-Type: text/csv

timestamp_ms,ax_g,mag_g
61317,0.00744,1.00659
61327,0.00640,1.00526
...
```

Chaque paquet commence par la ligne des noms de colonnes, puis 50 mesures.
Le numero d'envoi est tire au hasard au demarrage du Pico : tous les paquets
d'un meme demarrage vont dans le meme fichier. Reponse : `OK`.
