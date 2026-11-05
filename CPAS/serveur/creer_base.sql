-- Base de donnees des mesures du Pico. A importer dans phpMyAdmin (onglet Importer).
-- Attention : la table mesures est effacee puis recreee (elle repart vide).

CREATE DATABASE IF NOT EXISTS cpas_sante;
USE cpas_sante;

DROP TABLE IF EXISTS mesures;
CREATE TABLE mesures (
    id           INT AUTO_INCREMENT PRIMARY KEY,   -- numero de la ligne
    timestamp_ms INT,      -- temps depuis l'allumage du Pico (ms)
    ax_g         DOUBLE,   -- acceleration sur x (g)
    ay_g         DOUBLE,
    az_g         DOUBLE,
    mag_g        DOUBLE,   -- acceleration totale (g)
    gx_dps       DOUBLE,   -- vitesse de rotation sur x (degres par seconde)
    gy_dps       DOUBLE,
    gz_dps       DOUBLE
);
