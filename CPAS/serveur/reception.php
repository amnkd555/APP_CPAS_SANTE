<?php
// Recoit un paquet de mesures envoye par le Pico et l'enregistre dans MySQL.
// Le corps de la requete contient une mesure par ligne :
// 61317,0.00744,-0.05862,1.00485,1.00659,-0.140,0.184,-0.604
require 'config.php';

$pdo = connexion();
$requete = $pdo->prepare('INSERT INTO mesures (timestamp_ms, ax_g, ay_g, az_g, mag_g, gx_dps, gy_dps, gz_dps)'
                       . ' VALUES (?, ?, ?, ?, ?, ?, ?, ?)');

$texte = file_get_contents('php://input');            // le corps de la requete
foreach (explode("\n", trim($texte)) as $ligne) {
    $valeurs = explode(',', trim($ligne));            // "61317,0.00744,..." -> [61317, 0.00744, ...]
    $requete->execute($valeurs);                      // une ligne ajoutee dans la table
}

echo 'OK';
