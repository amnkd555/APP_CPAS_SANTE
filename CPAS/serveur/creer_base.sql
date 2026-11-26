-- Base de donnees du projet. A importer une fois dans phpMyAdmin (onglet Importer).
-- La table envois est la liste des fichiers CSV recus (les mesures sont dans les fichiers).
-- Rien n'est efface : on peut l'importer meme si la base existe deja.

CREATE DATABASE IF NOT EXISTS cpas_sante;
USE cpas_sante;

CREATE TABLE IF NOT EXISTS envois (
    id        INT AUTO_INCREMENT PRIMARY KEY,
    numero    BIGINT,                               -- numero d'envoi tire au hasard par le Pico
    fichier   VARCHAR(255),                         -- ex. data/2026-11-26_08-40-12_envoi3735928559.csv
    colonnes  VARCHAR(255),                         -- ex. timestamp_ms,ax_g,mag_g
    nb_lignes INT,                                  -- nombre de mesures dans le fichier
    recu_le   DATETIME DEFAULT CURRENT_TIMESTAMP    -- date du premier paquet
);
