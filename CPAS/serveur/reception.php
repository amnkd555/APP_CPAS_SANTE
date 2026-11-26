<?php
// Recoit un paquet du Pico et l'ajoute a un fichier CSV.
// Requete : POST reception.php?envoi=123456
// Corps   : 1re ligne = noms des colonnes, puis une mesure par ligne :
//           timestamp_ms,ax_g,mag_g
//           61317,0.00744,1.00659
// Tous les paquets d'un meme envoi vont dans le meme fichier, range dans le dossier data/.
// La table envois garde la liste des fichiers (nom, colonnes, nombre de lignes).
require 'config.php';

$numero = (int) $_GET['envoi'];                                   // (int) : que des chiffres dans le nom du fichier
$lignes = explode("\n", trim(file_get_contents('php://input')));
$colonnes = trim($lignes[0]);                                     // la ligne de titres
$mesures = array_slice($lignes, 1);                               // les mesures

$pdo = connexion();

// Le fichier de cet envoi existe-t-il deja ?
$requete = $pdo->prepare('SELECT fichier FROM envois WHERE numero = ?');
$requete->execute([$numero]);
$fichier = $requete->fetchColumn();

// Premier paquet de l'envoi : nouveau fichier qui commence par les titres
if ($fichier === false) {
    if (!is_dir('data')) {
        mkdir('data');
    }
    $fichier = 'data/' . date('Y-m-d_H-i-s') . "_envoi$numero.csv";
    file_put_contents($fichier, $colonnes . "\n");
    $requete = $pdo->prepare('INSERT INTO envois (numero, fichier, colonnes, nb_lignes) VALUES (?, ?, ?, 0)');
    $requete->execute([$numero, $fichier, $colonnes]);
}

// On ajoute les mesures a la fin du fichier
file_put_contents($fichier, implode("\n", $mesures) . "\n", FILE_APPEND);
$requete = $pdo->prepare('UPDATE envois SET nb_lignes = nb_lignes + ? WHERE numero = ?');
$requete->execute([count($mesures), $numero]);

echo 'OK';
