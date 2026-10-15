# Mesure de référence : capteur à plat

Fichier : `reference_plat.csv`. Capteur à plat, immobile, loin de tout moteur. 30 s, 104 mesures par seconde, le 21 octobre 2026.

## Résultats (moyennes sur les 3146 mesures)

| Valeur | Moyenne | Écart-type |
|---|---|---|
| ax | 0,007 g | 0,0006 g |
| ay | -0,057 g | 0,0006 g |
| az | 1,004 g | 0,0005 g |
| mag | 1,006 g | 0,0005 g |
| gx | -0,14 degré/s | 0,02 degré/s |
| gy | 0,17 degré/s | 0,06 degré/s |
| gz | -0,63 degré/s | 0,03 degré/s |

## Pourquoi ce résultat est logique

Le capteur est à plat et immobile : seule la pesanteur agit, verticalement (axe z).

- az = 1,00 g : c'est la pesanteur. ax et ay sont proches de 0 g : rien n'agit à l'horizontale.
- ay = -0,06 g : le capteur n'est pas parfaitement à plat (environ 3 degrés), c'est normal.
- gx, gy, gz entre -0,7 et 0,2 degré/s : pas de rotation. Ce petit décalage est normal pour un gyroscope.
- Bruit très faible (écart-type de 0,0006 g au plus) : la mesure est stable.

## Ce que la moyenne apporte

Chaque mesure tremble un peu, c'est le bruit. La moyenne efface ce tremblement et donne la vraie valeur du capteur au repos. C'est la **valeur de référence**, sans moteur.

Pour les tests suivants, robot allumé, on compare à cette référence :

- si la moyenne change, les moteurs faussent le capteur ;
- si l'écart-type augmente (les mesures tremblent plus), les moteurs perturbent le capteur.

## La magnitude (mag)

La magnitude est l'accélération totale, calculée à partir des trois axes : racine(ax² + ay² + az²). Comme les trois axes sont combinés, elle ne dépend pas de la façon dont le capteur est tourné.

- **Capteur immobile** : il ne reste que la pesanteur, donc mag vaut 1 g, que le capteur soit à plat, de côté ou à l'envers.
- **Capteur qui bouge** : mag s'écarte de 1 g. C'est donc elle qui permettra de savoir si la personne est active, quelle que soit la façon dont elle porte le boîtier. C'est la base du futur score MET.

Ici, mag vaut 1,00 g : le capteur était bien immobile.

## Formules utilisées

Notations : x₁, x₂, …, xₙ sont les n mesures d'un axe (ici n = 3146).

**Moyenne** : la somme des mesures divisée par leur nombre.

$$\bar{x} = \frac{x_1 + x_2 + \dots + x_n}{n}$$

**Écart-type** : de combien les mesures s'éloignent en moyenne de la moyenne. C'est la mesure du bruit.

$$\sigma = \sqrt{\frac{(x_1 - \bar{x})^2 + (x_2 - \bar{x})^2 + \dots + (x_n - \bar{x})^2}{n - 1}}$$

On divise par n - 1 et non par n : c'est l'écart-type d'un échantillon (fonction `statistics.stdev` de Python).

**Magnitude** : l'accélération totale, pour une mesure.

$$mag = \sqrt{a_x^2 + a_y^2 + a_z^2}$$
