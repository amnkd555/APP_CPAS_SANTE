#!/usr/bin/env python3
"""Affiche en direct le bruit du LSM6DSOX (écart-type par axe), sans rien enregistrer.

    python3 APP/outils/scripts/direct.py
    python3 APP/outils/scripts/direct.py --fenetre 10

Chaque seconde : écart-type de x, y, z sur les dernières secondes (mg) et nombre d'erreurs I2C.
Pratique pour voir le bruit changer quand on allume ou éteint le robot. Ctrl+C pour arrêter.
Aucune dépendance externe : bibliothèque standard Python uniquement.
"""

import argparse
import os
import select
import statistics
import termios
import time
import tty

from perturbations import trouver_port


def ouvrir_port(port):
    fd = os.open(port, os.O_RDWR | os.O_NOCTTY | os.O_NONBLOCK)
    tty.setraw(fd)
    attrs = termios.tcgetattr(fd)
    attrs[4] = attrs[5] = termios.B115200
    termios.tcsetattr(fd, termios.TCSANOW, attrs)
    return fd


def lire_points(fd, tampon):
    # Renvoie les mesures (x, y, z) complètes reçues, le reste du tampon et le nombre d'erreurs I2C
    points, erreurs = [], 0
    tampon += os.read(fd, 4096)
    *completes, tampon = tampon.split(b"\n")
    for brute in completes:
        ligne = brute.decode(errors="replace").strip()
        champs = ligne.split(",")
        if ligne.startswith("# erreur I2C"):
            erreurs += 1
        elif len(champs) >= 4 and ligne[0].isdigit():
            points.append((time.time(), [float(v) for v in champs[1:4]]))
    return points, tampon, erreurs


def afficher(points, erreurs):
    sigmas = [statistics.stdev(p[1][i] for p in points) * 1000 for i in range(3)]
    print(f"{time.strftime('%H:%M:%S')} | " + " ".join(f"{s:7.3f}" for s in sigmas) + f" | {erreurs:3d}")


def direct(port, fenetre):
    fd = ouvrir_port(port)
    print(f"En direct sur {port}, bruit sur les {fenetre:g} dernières s (mg). Ctrl+C pour arrêter.")
    print(f"{'heure':>8} | {'sx':>7} {'sy':>7} {'sz':>7} | {'err':>3}")
    points, erreurs, tampon = [], 0, b""
    prochain = time.time() + 1
    try:
        while True:
            if select.select([fd], [], [], 0.2)[0]:
                nouveaux, tampon, err = lire_points(fd, tampon)
                points += nouveaux
                erreurs += err
            maintenant = time.time()
            points = [p for p in points if p[0] > maintenant - fenetre]
            if maintenant >= prochain and len(points) > 1:
                prochain = maintenant + 1
                afficher(points, erreurs)
    except KeyboardInterrupt:
        print()
    finally:
        os.close(fd)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--fenetre", type=float, default=5, help="secondes prises pour le bruit")
    parser.add_argument("--port", default=None)
    args = parser.parse_args()
    direct(args.port or trouver_port(), args.fenetre)


if __name__ == "__main__":
    main()
