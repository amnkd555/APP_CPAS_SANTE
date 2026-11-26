// Dessine un graphique par colonne de mesures avec Chart.js.
// TITRES = noms des colonnes, POINTS = mesures (une liste de valeurs par ligne), ecrits par index.php.
// Si la 1re colonne est timestamp_ms, elle sert d'axe du temps ; sinon on numerote les mesures.

const avecTemps = TITRES[0] === 'timestamp_ms';
const zone = document.getElementById('graphiques');

TITRES.forEach((nom, colonne) => {
  if (avecTemps && colonne === 0) return;   // pas de graphique pour le temps lui-meme

  // Un titre et une zone de dessin par colonne
  const titre = document.createElement('h2');
  titre.textContent = nom;
  const cadre = document.createElement('div');
  cadre.className = 'graphique';
  const dessin = document.createElement('canvas');
  cadre.appendChild(dessin);
  zone.append(titre, cadre);

  new Chart(dessin, {
    type: 'line',
    data: {
      datasets: [{
        label: nom,
        borderColor: 'blue',
        pointRadius: 0,
        data: POINTS.map((m, i) => ({ x: avecTemps ? m[0] / 1000 : i * PAS, y: Number(m[colonne]) })),
      }],
    },
    options: {
      animation: false,
      maintainAspectRatio: false,
      plugins: { legend: { display: false } },
      scales: { x: { type: 'linear', title: { display: true, text: avecTemps ? 'temps (s)' : 'numero de la mesure' } } },
    },
  });
});
