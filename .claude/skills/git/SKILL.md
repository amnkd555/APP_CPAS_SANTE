---
name: git
description: Règles Git du projet — branches main/develop/feature/fix, worktree, format des commits (feat:, fix:…), 4 fichiers max par commit, merge vers main seulement depuis develop, vers develop seulement depuis feature/fix. À utiliser avant toute création de branche, de worktree ou de commit.
---

# Git — règles du projet

## Les branches

```
main      versions stables   (aucun commit ; merge uniquement depuis develop)
  develop intégration        (aucun commit ; merge uniquement depuis une branche de travail)
    feature/<nom>   nouvelle fonctionnalité, créée depuis develop
    fix/<nom>       correction de bug, créée depuis develop
    (aussi : hotfix/, chore/, docs/, refactor/, test/)
```

1. Créer la branche depuis `develop` : `git switch develop && git pull && git switch -c feature/<nom>`.
2. Commiter sur cette branche (jamais sur `main` ni `develop`).
3. Pousser la branche et ouvrir une pull request **vers `develop`** : `gh pr create --base develop --fill`.
4. Merges autorisés : branche de travail → `develop`, puis `develop` → `main`. Tout autre merge vers `main`/`develop` est bloqué (`git merge` et `gh pr merge`).

Nom de branche : minuscules, mots séparés par `-`, sans accents (`feature/carte-sd`, `fix/bus-i2c-bloque`). Une branche = un seul sujet.

## Worktree

Une 2e copie du dépôt sur une autre branche, dans un autre dossier.

```sh
git worktree add .claude/worktrees/carte-sd -b feature/carte-sd develop   # créer
git worktree list                                                         # lister
git worktree remove .claude/worktrees/carte-sd                            # supprimer (après merge)
```

- Dossier : `.claude/worktrees/<nom>` (ignoré par Git).
- Ne pas copier `APP/outils/build/` (chemins absolus) : reconfigurer avec `cmake -S APP/outils -B APP/outils/build -G Ninja`.

## Commit

Format : `<type>(<portee>): <description>` — portée facultative, description à l'impératif, minuscule, sans point final, ≤ 72 caractères.

| Type | Usage | Exemple |
|---|---|---|
| `feat` | Nouvelle fonctionnalité | `feat(capteur): ajoute la lecture du gyroscope` |
| `fix` | Correction de bug | `fix(bus): corrige le délai I2C` |
| `docs` | README, docs/, consignes | `docs: complete le README` |
| `refactor` | Réorganisation du code | `refactor(acquisition): découpe la boucle` |
| `test` | Tests | `test(capteur): teste l'erreur I2C` |
| `chore` | Outils, config, ménage | `chore: met à jour le hook git` |
| `ci` | GitHub Actions | `ci: compile le firmware à chaque push` |
| `build` | CMake, toolchain | `build: passe au SDK 2.2.0` |

Portées : `bus`, `capteur`, `acquisition`, `outils`.

- **4 fichiers max par commit.** Au-delà, découper (ex. `APP/capteur.c` + `APP/capteur.h` + `APP/docs/capteur.md` ensemble, le test à part).
- Un commit = un seul changement logique.
- Le code doit compiler et les tests passer (`./APP/outils/scripts/verifier.sh </dev/null`).
- Ajouter les fichiers par nom (`git add bus.c bus.h`), jamais `git add .`.
- `git add` et `git commit` dans deux commandes séparées (pas de `&&`, pas de `commit -a`).
- Jamais de secret (jeton, mot de passe) ni d'emoji.
- Jamais de `Co-Authored-By` ni de mention de Claude dans les commits et les PR.
- Vérifié par le hook `.claude/hooks/verif_git.sh` : commande bloquée si non respecté.

```sh
git status --short
git diff --cached --name-only | wc -l    # doit être <= 4
```

## Fin de branche

```sh
git push -u origin feature/carte-sd
gh pr create --base develop --fill
```

Après le merge : `git switch develop && git pull && git branch -d feature/carte-sd`.
