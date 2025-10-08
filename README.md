# INF231-TP3
#  DESCRIPTION DU PROGRAMME
##  Objectif du projet
Ce projet a pour but de créer une mini-application de traitement d’images au format PPM (Portable PixMap) à l’aide du langage C sous Linux/Debian/Ubuntu.
Le programme permet de :

- **charger une image existante**

- **Effectuer plusieurs opérations de traitement** (négatif, niveaux de gris, découpe, etc.)

**L’application fonctionne en ligne de commande interactive :**
L’utilisateur tape des instructions et voit le résultat immédiatement.

---
##  Structure générale du projet

Le projet est constituer de deux fichiers; `ppm.c` et `ppm.h`.
>Sur GitHUb, nous avons inclue le fichier executable (`ppm`) du programme


##  Principe de fonctionnement

1. L’utilisateur insere le fichier.ppm dans le dossier source du programme (Le fichier.ppm doit etre en format P3.).


2. Il choisit une opération à effectuer sur cette image (gris, neg, dom, etc.).


3. L’image modifiée est enregistrée sous `out.ppm` et pour verifier, il suffit d'aller dans le dossier source.


4. L’utilisateur peut enchaîner plusieurs traitements.

---


#  EXÉCUTION DU PROGRAMME

>  Assurez-vous d’être sous Linux/Debian/Ubuntu et d’avoir installé gcc et xdg-utils.

### 1 — Importation du fichier.ppm 

Dans le dossier ou le code source ce trouve, il faut y mettre le fichier.ppm. Et il sera plus facile pour le prgramme de detecter le fichier en question.

---

### 2 — Compiler le programme principal
#### Etape 1:

Depuis le terminal de Debian/Ubuntu, acceder au dossier ou se trouve le code source avec cette comande:
```
cd nom_dossier
```
Et si le dossier avec le code source se trouve dans un autre dossier, tapez
```
cd dossier_numero1/dossier_numero2
```
et le faire jusqu'a ce que vous acceder au dossier avec le code sources

#### Etapes 2
Compile les fichiers :
```
gcc ppm.c -o ppmviewer
```

Cela génère l’exécutable **ppmviewer.**


---

### 3 — Lancer le programme principal
```
./ppmviewer
```


# UTILISATION INTERACTIVE (commandes et exemples)

Apres avoir executer, vous verrez ceci:
```
==== PPM Image Viewer ===
Tapez 'help' pour voir les commandes disponibles

ppmviewer> 
```
Tapez les commandes qui s'afficheront une par une.
Chaque commande produit une image **out.ppm**.

---
