// Dessine les 3 graphiques avec Chart.js, a partir de MESURES (ecrit par index.php).
// MESURES = liste des lignes de la table : [{id: .., timestamp_ms: .., ax_g: .., ...}, ...]

// Une courbe : le temps (s) en x, la colonne "nom" en y
function courbe(nom, couleur) {
  return {
    label: nom,
    borderColor: couleur,
    pointRadius: 0,
    data: MESURES.map(m => ({ x: m.timestamp_ms / 1000, y: Number(m[nom]) })),
  };
}

// Un graphique dans le <canvas> qui a cet id
function graphique(id, courbes) {
  new Chart(document.getElementById(id), {
    type: 'line',
    data: { datasets: courbes },
    options: {
      animation: false,
      maintainAspectRatio: false,
      scales: { x: { type: 'linear', title: { display: true, text: 'temps (s)' } } },
    },
  });
}

graphique('graphique_mag', [courbe('mag_g', 'blue')]);
graphique('graphique_acc', [courbe('ax_g', 'blue'), courbe('ay_g', 'orange'), courbe('az_g', 'green')]);
graphique('graphique_gyro', [courbe('gx_dps', 'blue'), courbe('gy_dps', 'orange'), courbe('gz_dps', 'green')]);
