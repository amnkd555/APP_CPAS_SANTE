<?php
// Page de resultats : choix d'un envoi, un graphique par colonne, 20 dernieres mesures.
require 'config.php';

$pdo = connexion();
$envois = $pdo->query('SELECT * FROM envois ORDER BY id DESC')->fetchAll(PDO::FETCH_ASSOC);

// L'envoi affiche : celui choisi dans la liste, sinon le plus recent
$choisi = $envois[0] ?? null;
foreach ($envois as $e) {
    if (isset($_GET['envoi']) && $e['numero'] == $_GET['envoi']) {
        $choisi = $e;
    }
}

// Lecture du fichier CSV de cet envoi
$titres = [];
$mesures = [];
if ($choisi !== null) {
    $contenu = file($choisi['fichier'], FILE_IGNORE_NEW_LINES);
    $titres = explode(',', $contenu[0]);
    foreach (array_slice($contenu, 1) as $ligne) {
        $mesures[] = explode(',', $ligne);
    }
}

// Pour les graphiques : au plus 2000 points (une mesure sur "pas")
$pas = max(1, intdiv(count($mesures), 2000));
$points = [];
for ($i = 0; $i < count($mesures); $i += $pas) {
    $points[] = $mesures[$i];
}
?>
<!doctype html>
<html lang="fr">
<head>
<meta charset="utf-8">
<title>CPAS Sante - mesures du Pico</title>
<link rel="stylesheet" href="style.css">
<script src="chart.umd.min.js"></script>
</head>
<body>
<h1>Mesures du Pico</h1>

<?php if ($choisi === null): ?>
  <p>Aucun envoi pour l'instant.</p>
<?php else: ?>
  <form method="get">
    <select name="envoi" onchange="this.form.submit()">
      <?php foreach ($envois as $e): ?>
        <option value="<?= $e['numero'] ?>" <?= $e === $choisi ? 'selected' : '' ?>>
          <?= $e['recu_le'] ?> - <?= $e['nb_lignes'] ?> mesures - <?= $e['colonnes'] ?>
        </option>
      <?php endforeach; ?>
    </select>
  </form>
  <p><?= count($mesures) ?> mesures - <a href="<?= $choisi['fichier'] ?>">telecharger le fichier CSV</a></p>

  <div id="graphiques"></div>

  <h2>20 dernieres mesures</h2>
  <table>
    <tr><?php foreach ($titres as $t): ?><th><?= $t ?></th><?php endforeach; ?></tr>
    <?php foreach (array_slice($mesures, -20) as $m): ?>
      <tr><?php foreach ($m as $valeur): ?><td><?= $valeur ?></td><?php endforeach; ?></tr>
    <?php endforeach; ?>
  </table>

  <script>
    const TITRES = <?= json_encode($titres) ?>;
    const POINTS = <?= json_encode($points) ?>;
    const PAS = <?= $pas ?>;
  </script>
  <script src="graphiques.js"></script>
<?php endif; ?>
</body>
</html>
