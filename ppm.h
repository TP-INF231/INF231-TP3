#ifndef PPM_H
#define PPM_H

#include <stdio.h>
#include <stdlib.h>

// Structure pour représenter un pixel RGB
typedef struct {
    unsigned char r;  // Rouge (0-255)
    unsigned char g;  // Vert (0-255)
    unsigned char b;  // Bleu (0-255)
} Pixel;

// Structure pour représenter une image PPM
typedef struct {
    int largeur;
    int hauteur;
    int valeur_max;  // Valeur maximale des couleurs (généralement 255)
    Pixel **pixels;  // Tableau 2D de pixels
} ImagePPM;

// Fonctions pour créer et libérer une image
ImagePPM* creer_image(int largeur, int hauteur);
void liberer_image(ImagePPM *img);

// Fonctions pour définir et obtenir des pixels
void definir_pixel(ImagePPM *img, int x, int y, unsigned char r, unsigned char g, unsigned char b);
Pixel obtenir_pixel(ImagePPM *img, int x, int y);

// Fonctions pour écrire et lire des fichiers PPM
int ecrire_ppm(const char *nom_fichier, ImagePPM *img);
ImagePPM* lire_ppm(const char *nom_fichier);

// Fonction pour afficher les informations de l'image
void afficher_info_image(ImagePPM *img);
void afficher_image_ascii(ImagePPM *img);

#endif