#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned char r, g, b;
} Pixel;

typedef struct {
    int width, height, max_val;
    Pixel **data;
} Image;

// Fonction pour allouer une image
Image* allouer_image(int w, int h, int max) {
    Image *img = malloc(sizeof(Image));
    img->width = w;
    img->height = h;
    img->max_val = max;
    img->data = malloc(h * sizeof(Pixel*));
    for (int i = 0; i < h; i++) {
        img->data[i] = malloc(w * sizeof(Pixel));
    }
    return img;
}

// Fonction pour libérer la mémoire d'une image
void liberer_image(Image *img) {
    for (int i = 0; i < img->height; i++) {
        free(img->data[i]);
    }
    free(img->data);
    free(img);
}

// Fonction pour lire une image PPM (P3 ou P6)
Image* lire_ppm(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        printf("Erreur: impossible d'ouvrir %s\n", filename);
        return NULL;
    }

    char magic[3];
    int w, h, max;
    if (fscanf(f, "%2s", magic) != 1) {
        printf("Erreur: impossible de lire l'en-tête du fichier\n");
        fclose(f);
        return NULL;
    }

    // Ignorer les commentaires (# ...)
    int c;
    while ((c = fgetc(f)) == '#') {
        while ((c = fgetc(f)) != '\n' && c != EOF);
    }
    ungetc(c, f);

    // Lire largeur, hauteur, valeur max
    if (fscanf(f, "%d %d %d", &w, &h, &max) != 3) {
        printf("Erreur: en-tête PPM invalide\n");
        fclose(f);
        return NULL;
    }

    // Consommer le dernier saut de ligne
    fgetc(f);

    // Allouer l'image
    Image *img = allouer_image(w, h, max);

    if (strcmp(magic, "P6") == 0) {
        // Lecture binaire
        for (int i = 0; i < h; i++) {
            fread(img->data[i], sizeof(Pixel), w, f);
        }
    } 
    else if (strcmp(magic, "P3") == 0) {
        // Lecture texte
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                if (fscanf(f, "%hhu %hhu %hhu", 
                           &img->data[i][j].r,
                           &img->data[i][j].g,
                           &img->data[i][j].b) != 3) {
                    printf("Erreur: données P3 invalides\n");
                    liberer_image(img);
                    fclose(f);
                    return NULL;
                }
            }
        }
    } 
    else {
        printf("Erreur: format PPM inconnu (%s)\n", magic);
        liberer_image(img);
        fclose(f);
        return NULL;
    }

    fclose(f);
    return img;
}

// Fonction pour écrire une image PPM
void ecrire_ppm(const char *filename, Image *img) {
    FILE *f = fopen(filename, "wb");
    if (!f) {
        printf("Erreur: impossible de créer %s\n", filename);
        return;
    }
    
    fprintf(f, "P6\n%d %d\n%d\n", img->width, img->height, img->max_val);
    
    for (int i = 0; i < img->height; i++) {
        fwrite(img->data[i], sizeof(Pixel), img->width, f);
    }
    
    fclose(f);
}

// 1. Foncer/Éclaircir les pixels dominants (R, G ou B)
void modifier_dominantes(Image *img, char couleur, int valeur) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            Pixel *p = &img->data[i][j];
            
            int dominant = 0;
            
            // Déterminer si la couleur spécifiée est dominante
            switch(couleur) {
                case 'R':
                    if (p->r > p->g && p->r > p->b) dominant = 1;
                    break;
                case 'G':
                    if (p->g > p->r && p->g > p->b) dominant = 1;
                    break;
                case 'B':
                    if (p->b > p->r && p->b > p->g) dominant = 1;
                    break;
            }
            
            if (dominant) {
                // Modifier toutes les composantes du pixel
                int new_r = p->r + valeur;
                int new_g = p->g + valeur;
                int new_b = p->b + valeur;
                
                p->r = (new_r < 0) ? 0 : (new_r > img->max_val ? img->max_val : new_r);
                p->g = (new_g < 0) ? 0 : (new_g > img->max_val ? img->max_val : new_g);
                p->b = (new_b < 0) ? 0 : (new_b > img->max_val ? img->max_val : new_b);
            }
        }
    }
}

// 2. Passer en noir et blanc
void noir_et_blanc(Image *img) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            Pixel *p = &img->data[i][j];
            unsigned char gris = (p->r + p->g + p->b) / 3;
            p->r = p->g = p->b = gris;
        }
    }
}

// 3. Afficher la taille
void afficher_taille(Image *img) {
    printf("Taille: %d x %d pixels\n", img->width, img->height);
}

// 4. Extraire une partie de l'image
Image* extraire_partie(Image *img, int l1, int l2, int c1, int c2) {
    if (l1 < 0 || l2 >= img->height || c1 < 0 || c2 >= img->width || l1 > l2 || c1 > c2) {
        printf("Erreur: coordonnées invalides\n");
        printf("Les valeurs doivent être: 0 <= l1 <= l2 < %d et 0 <= c1 <= c2 < %d\n", 
               img->height, img->width);
        return NULL;
    }
    
    int new_h = l2 - l1 + 1;
    int new_w = c2 - c1 + 1;
    Image *extrait = allouer_image(new_w, new_h, img->max_val);
    
    for (int i = 0; i < new_h; i++) {
        for (int j = 0; j < new_w; j++) {
            extrait->data[i][j] = img->data[l1 + i][c1 + j];
        }
    }
    
    return extrait;
}

// 5. Créer le négatif
void negatif(Image *img) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            Pixel *p = &img->data[i][j];
            p->r = img->max_val - p->r;
            p->g = img->max_val - p->g;
            p->b = img->max_val - p->b;
        }
    }
}

// 6. Filtre médian (8 voisins)
void filtre_median(Image *img) {
    Image *copie = allouer_image(img->width, img->height, img->max_val);
    
    // Copier l'image originale
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            copie->data[i][j] = img->data[i][j];
        }
    }
    
    // Fonction de comparaison pour qsort
    int comparer(const void *a, const void *b) {
        return (*(unsigned char*)a - *(unsigned char*)b);
    }
    
    for (int i = 1; i < img->height - 1; i++) {
        for (int j = 1; j < img->width - 1; j++) {
            unsigned char r[9], g[9], b[9];
            int k = 0;
            
            // Collecter les 9 pixels (centre + 8 voisins)
            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    r[k] = copie->data[i+di][j+dj].r;
                    g[k] = copie->data[i+di][j+dj].g;
                    b[k] = copie->data[i+di][j+dj].b;
                    k++;
                }
            }
            
            // Trier et prendre la médiane
            qsort(r, 9, sizeof(unsigned char), comparer);
            qsort(g, 9, sizeof(unsigned char), comparer);
            qsort(b, 9, sizeof(unsigned char), comparer);
            
            img->data[i][j].r = r[4];
            img->data[i][j].g = g[4];
            img->data[i][j].b = b[4];
        }
    }
    
    liberer_image(copie);
}

void afficher_aide() {
    printf("\nCommandes disponibles:\n");
    printf("  dom c val fichier.ppm        - Foncer/Éclaircir les pixels dominants\n");
    printf("                                 c = R, G ou B; val = entier (positif ou négatif)\n");
    printf("  gris fichier.ppm             - Passer en noir et blanc\n");
    printf("  size fichier.ppm             - Afficher la taille\n");
    printf("  cut fichier.ppm l1 l2 c1 c2 fichier_resultat.ppm - Découper l'image\n");
    printf("  neg fichier.ppm fichier_resultat.ppm - Créer le négatif\n");
    printf("  fil fichier.ppm fichier_resultat.ppm - Appliquer le filtre médian\n");
    printf("  quit                         - Quitter l'application\n\n");
}

int main() {
    printf("=== PPM Image Viewer ===\n");
    printf("Tapez 'help' pour voir les commandes disponibles\n\n");
    
    char commande[256];
    
    while (1) {
        printf("ppmviewer> ");
        
        if (!fgets(commande, sizeof(commande), stdin)) {
            break;
        }
        
        // Enlever le newline
        commande[strcspn(commande, "\n")] = 0;
        
        // Découper la commande
        char cmd[20];
        sscanf(commande, "%s", cmd);
        
        if (strcmp(cmd, "quit") == 0) {
            printf("Au revoir!\n");
            break;
        }
        else if (strcmp(cmd, "help") == 0) {
            afficher_aide();
        }
        else if (strcmp(cmd, "dom") == 0) {
            char couleur, fichier[256];
            int val;
            
            if (sscanf(commande, "dom %c %d %s", &couleur, &val, fichier) != 3) {
                printf("Erreur: syntaxe incorrecte. Usage: dom c val fichier.ppm\n");
                continue;
            }
            
            if (couleur != 'R' && couleur != 'G' && couleur != 'B') {
                printf("Erreur: la couleur doit être R, G ou B\n");
                continue;
            }
            
            Image *img = lire_ppm(fichier);
            if (img) {
                modifier_dominantes(img, couleur, val);
                ecrire_ppm(fichier, img);
                printf("Image modifiée et sauvegardée: %s\n", fichier);
                liberer_image(img);
            }
        }
        else if (strcmp(cmd, "gris") == 0) {
            char fichier[256];
            
            if (sscanf(commande, "gris %s", fichier) != 1) {
                printf("Erreur: syntaxe incorrecte. Usage: gris fichier.ppm\n");
                continue;
            }
            
            Image *img = lire_ppm(fichier);
            if (img) {
                noir_et_blanc(img);
                ecrire_ppm(fichier, img);
                printf("Image convertie en noir et blanc: %s\n", fichier);
                liberer_image(img);
            }
        }
        else if (strcmp(cmd, "size") == 0) {
            char fichier[256];
            
            if (sscanf(commande, "size %s", fichier) != 1) {
                printf("Erreur: syntaxe incorrecte. Usage: size fichier.ppm\n");
                continue;
            }
            
            Image *img = lire_ppm(fichier);
            if (img) {
                afficher_taille(img);
                liberer_image(img);
            }
        }
        else if (strcmp(cmd, "cut") == 0) {
            char fichier_in[256], fichier_out[256];
            int l1, l2, c1, c2;
            
            if (sscanf(commande, "cut %s %d %d %d %d %s", 
                       fichier_in, &l1, &l2, &c1, &c2, fichier_out) != 6) {
                printf("Erreur: syntaxe incorrecte. Usage: cut fichier.ppm l1 l2 c1 c2 fichier_resultat.ppm\n");
                continue;
            }
            
            Image *img = lire_ppm(fichier_in);
            if (img) {
                Image *extrait = extraire_partie(img, l1, l2, c1, c2);
                if (extrait) {
                    ecrire_ppm(fichier_out, extrait);
                    printf("Partie extraite et sauvegardée: %s\n", fichier_out);
                    liberer_image(extrait);
                }
                liberer_image(img);
            }
        }
        else if (strcmp(cmd, "neg") == 0) {
            char fichier_in[256], fichier_out[256];
            
            if (sscanf(commande, "neg %s %s", fichier_in, fichier_out) != 2) {
                printf("Erreur: syntaxe incorrecte. Usage: neg fichier.ppm fichier_resultat.ppm\n");
                continue;
            }
            
            Image *img = lire_ppm(fichier_in);
            if (img) {
                negatif(img);
                ecrire_ppm(fichier_out, img);
                printf("Négatif créé et sauvegardé: %s\n", fichier_out);
                liberer_image(img);
            }
        }
        else if (strcmp(cmd, "fil") == 0) {
            char fichier_in[256], fichier_out[256];
            
            if (sscanf(commande, "fil %s %s", fichier_in, fichier_out) != 2) {
                printf("Erreur: syntaxe incorrecte. Usage: fil fichier.ppm fichier_resultat.ppm\n");
                continue;
            }
            
            Image *img = lire_ppm(fichier_in);
            if (img) {
                filtre_median(img);
                ecrire_ppm(fichier_out, img);
                printf("Filtre médian appliqué et sauvegardé: %s\n", fichier_out);
                liberer_image(img);
            }
        }
        else {
            printf("Commande inconnue: %s\n", cmd);
            printf("Tapez 'help' pour voir les commandes disponibles\n");
        }
    }
    
    return 0;
}