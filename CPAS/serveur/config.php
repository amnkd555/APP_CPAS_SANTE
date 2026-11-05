<?php
// Connexion a MySQL. Valeurs par defaut de XAMPP : utilisateur root, sans mot de passe.
function connexion(): PDO
{
    return new PDO('mysql:host=localhost;dbname=cpas_sante', 'root', '');
}
