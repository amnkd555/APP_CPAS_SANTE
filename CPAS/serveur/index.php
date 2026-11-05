<?php
// Page de resultats : graphiques et 20 dernieres mesures.
require 'config.php';

$pdo = connexion();
$mesures = $pdo->query('SELECT * FROM mesures ORDER BY id')->fetchAll(PDO::FETCH_ASSOC);
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
<p><?= count($mesures) ?> mesures dans la base</p>

<h2>Acceleration totale (mag_g)</h2>
<div class="graphique"><canvas id="graphique_mag"></canvas></div>
<h2>Accelerometre : 3 axes</h2>
<div class="graphique"><canvas id="graphique_acc"></canvas></div>
<h2>Gyroscope : 3 axes</h2>
<div class="graphique"><canvas id="graphique_gyro"></canvas></div>

<h2>20 dernieres mesures</h2>
<table>
  <tr><th>id</th><th>timestamp_ms</th><th>ax_g</th><th>ay_g</th><th>az_g</th><th>mag_g</th><th>gx_dps</th><th>gy_dps</th><th>gz_dps</th></tr>
  <?php foreach (array_slice($mesures, -20) as $m): ?>
    <tr>
      <td><?= $m['id'] ?></td><td><?= $m['timestamp_ms'] ?></td>
      <td><?= $m['ax_g'] ?></td><td><?= $m['ay_g'] ?></td><td><?= $m['az_g'] ?></td><td><?= $m['mag_g'] ?></td>
      <td><?= $m['gx_dps'] ?></td><td><?= $m['gy_dps'] ?></td><td><?= $m['gz_dps'] ?></td>
    </tr>
  <?php endforeach; ?>
</table>

<!-- Les mesures passent du PHP au JavaScript sous forme de JSON -->
<script>const MESURES = <?= json_encode($mesures) ?>;</script>
<script src="graphiques.js"></script>
</body>
</html>
