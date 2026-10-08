#!/usr/bin/env python3
"""Capture et analyse des mesures du test de perturbations du LSM6DSOX (robot UR3).

Capture (enregistre le flux CSV du Pico pendant N secondes) :
    python3 scripts/perturbations.py capture d050_on_r1 --duree 30

Analyse (compare robot hors tension / sous tension à chaque distance) :
    python3 scripts/perturbations.py analyse

Nom des fichiers : d<distance en mm>_<off|on>_r<répétition>.csv, ex. d050_on_r2.csv
Aucune dépendance externe : bibliothèque standard Python uniquement.
"""

import argparse
import glob
import os
import re
import select
import statistics
import sys
import termios
import time
import tty

DOSSIER = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "mesures")
AXES = ["ax_g", "ay_g", "az_g", "mag_g"]
MOTIF = re.compile(r"d(\d+)_(off|on)_r(\d+)\.csv$")


def trouver_port():
    ports = sorted(glob.glob("/dev/cu.usbmodem*"))
    if not ports:
        sys.exit("Aucun Pico détecté (/dev/cu.usbmodem*). Fermer le Serial Monitor de VS Code ?")
    return ports[0]


def capture(etiquette, duree, port):
    os.makedirs(DOSSIER, exist_ok=True)
    chemin = os.path.join(DOSSIER, etiquette + ".csv")
    if os.path.exists(chemin):
        sys.exit(f"{chemin} existe déjà : choisir une autre étiquette.")

    fd = os.open(port, os.O_RDWR | os.O_NOCTTY | os.O_NONBLOCK)
    tty.setraw(fd)
    attrs = termios.tcgetattr(fd)
    attrs[4] = attrs[5] = termios.B115200
    termios.tcsetattr(fd, termios.TCSANOW, attrs)

    lignes, erreurs, tampon = [], 0, b""
    print(f"Capture {duree} s sur {port} -> {chemin}")
    fin = time.time() + duree
    while time.time() < fin:
        prets, _, _ = select.select([fd], [], [], 0.5)
        if not prets:
            continue
        tampon += os.read(fd, 4096)
        *completes, tampon = tampon.split(b"\n")
        for brute in completes:
            ligne = brute.decode(errors="replace").strip()
            if ligne.startswith("# erreur I2C"):
                erreurs += 1
            elif ligne and ligne[0].isdigit():
                lignes.append(ligne)
    os.close(fd)

    with open(chemin, "w") as f:
        f.write("timestamp_ms," + ",".join(AXES) + "\n")
        f.write("\n".join(lignes) + "\n")
    with open(chemin + ".erreurs", "w") as f:
        f.write(f"{erreurs}\n")

    donnees = lire_csv(chemin)
    duree_reelle = (donnees["t"][-1] - donnees["t"][0]) / 1000 if len(donnees["t"]) > 1 else 0
    print(f"{len(lignes)} échantillons, {len(lignes) / max(duree_reelle, 1e-9):.1f} Hz, erreurs I2C : {erreurs}")
    for axe in AXES:
        print(f"  {axe:6s} moyenne {statistics.fmean(donnees[axe]):+.5f} g   "
              f"écart-type {statistics.stdev(donnees[axe]) * 1000:.3f} mg")


def lire_csv(chemin):
    donnees = {"t": [], **{axe: [] for axe in AXES}}
    with open(chemin) as f:
        next(f)
        for ligne in f:
            champs = ligne.strip().split(",")
            if len(champs) != 5:
                continue
            donnees["t"].append(int(champs[0]))
            for axe, valeur in zip(AXES, champs[1:]):
                donnees[axe].append(float(valeur))
    return donnees


def lire_erreurs(chemin):
    try:
        with open(chemin + ".erreurs") as f:
            return int(f.read().strip())
    except (OSError, ValueError):
        return 0


def analyse(seuil):
    # sigma[(distance, etat)][axe] = liste des écarts-types (une valeur par répétition)
    sigma, erreurs = {}, {}
    for chemin in sorted(glob.glob(os.path.join(DOSSIER, "*.csv"))):
        m = MOTIF.search(os.path.basename(chemin))
        if not m:
            continue
        cle = (int(m.group(1)), m.group(2))
        donnees = lire_csv(chemin)
        for axe in AXES:
            sigma.setdefault(cle, {}).setdefault(axe, []).append(statistics.stdev(donnees[axe]))
        erreurs[cle] = erreurs.get(cle, 0) + lire_erreurs(chemin)

    distances = sorted({d for d, _ in sigma if (d, "off") in sigma and (d, "on") in sigma})
    if not distances:
        sys.exit("Aucune paire off/on trouvée dans mesures/.")

    print(f"Critère : R = max sur x, y, z de sigma_on / sigma_off <= {seuil}, et 0 erreur I2C\n")
    print(f"{'d (mm)':>7} | {'sx off':>7} {'sx on':>7} | {'sy off':>7} {'sy on':>7} | "
          f"{'sz off':>7} {'sz on':>7} | {'R':>5} | {'err':>3} | verdict")
    ok = {}
    for d in distances:
        moy = {(etat, axe): statistics.fmean(sigma[(d, etat)][axe]) * 1000
               for etat in ("off", "on") for axe in AXES}
        r = max(moy[("on", axe)] / moy[("off", axe)] for axe in AXES[:3])
        err = erreurs.get((d, "on"), 0)
        ok[d] = r <= seuil and err == 0
        print(f"{d:7d} | " + " | ".join(f"{moy[('off', a)]:7.3f} {moy[('on', a)]:7.3f}" for a in AXES[:3])
              + f" | {r:5.2f} | {err:3d} | {'OK' if ok[d] else 'PERTURBE'}")

    # Distance minimale : la plus petite d à partir de laquelle TOUTES les distances suivantes sont OK
    d_min = None
    for d in reversed(distances):
        if not ok[d]:
            break
        d_min = d
    print()
    if d_min is None:
        print("Aucune distance ne satisfait le critère : élargir la plage de distances.")
    else:
        print(f"Distance minimale sans perturbation mesurable : {d_min} mm (écarts-types en mg)")


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sous = parser.add_subparsers(dest="commande", required=True)
    p_cap = sous.add_parser("capture")
    p_cap.add_argument("etiquette", help="ex. d050_on_r1")
    p_cap.add_argument("--duree", type=float, default=30)
    p_cap.add_argument("--port", default=None)
    p_ana = sous.add_parser("analyse")
    p_ana.add_argument("--seuil", type=float, default=1.10)
    args = parser.parse_args()

    if args.commande == "capture":
        capture(args.etiquette, args.duree, args.port or trouver_port())
    else:
        analyse(args.seuil)


if __name__ == "__main__":
    main()
