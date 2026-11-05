# CPAS - envoi des mesures du Pico vers le serveur XAMPP (version simple)

Le Pico 2 W envoie un fichier de mesures (accelerometre + gyroscope) a un serveur XAMPP.
Le serveur les range dans MySQL et les affiche sur une page web avec des graphiques.

```
CSV -> firmware du Pico (C) -> Wi-Fi -> Apache/PHP -> MySQL -> page HTML
```

Explication avec schemas : `docs/CPAS_Fonctionnement_Simple.pdf`.

Cette version est volontairement minimale : aucune verification d'erreur, aucun reessai.
Si quelque chose ne marche pas (Wi-Fi, serveur arrete, mauvaise IP), le Pico ne le signale pas.

## Contenu

| Fichier | Role |
|---|---|
| `pico/main.c` | connexion Wi-Fi, appel de `envoyer_csv()`, LED allumee a la fin |
| `pico/envoi.c` | parcourt le CSV et l'envoie par paquets de 50 lignes |
| `pico/http_post.c` | requete HTTP POST avec l'API TCP de lwIP |
| `pico/donnees/` | le CSV integre au firmware a la compilation (`csv_vers_c.cmake`) |
| `pico/config.h` | SSID, mot de passe Wi-Fi, IP du PC (modele : `config_exemple.h`) |
| `serveur/` | a copier dans `C:\xampp\htdocs\cpas\` |
| `serveur/creer_base.sql` | base `cpas_sante`, table `mesures` (a importer dans phpMyAdmin) |
| `serveur/reception.php` | recoit un paquet en POST et l'insere dans la base |
| `serveur/index.php` | graphiques Chart.js et 20 dernieres mesures |

## Demarrage rapide

1. `pico/config.h` : mettre le mot de passe Wi-Fi et l'IP du PC.
2. Compiler (sous WSL) : `./pico/compiler.sh` -> `~/build-cpas/cpas_envoi.uf2`.
3. Serveur : demarrer Apache et MySQL dans XAMPP, importer `serveur/creer_base.sql`
   (la table repart vide), copier `serveur/` dans `C:\xampp\htdocs\cpas\`.
4. Tester le serveur sans le Pico (PowerShell, dans `docs/`) :
   ```
   curl.exe --data-binary "@paquet_test.csv" "http://localhost/cpas/reception.php"
   ```
   puis ouvrir `http://localhost/cpas/`.
5. Flasher : BOOTSEL + brancher, glisser `cpas_envoi.uf2` sur le lecteur `RP2350`.

## Format envoye par le Pico

```
POST /cpas/reception.php HTTP/1.0
Content-Type: text/csv

61317,0.00744,-0.05862,1.00485,1.00659,-0.140,0.184,-0.604
61327,0.00640,-0.05685,1.00363,1.00526,-0.123,0.192,-0.674
...
```

Reponse : `OK`.
