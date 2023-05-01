#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#define min(i, j) (((i) < (j)) ? (i) : (j))
#define max(i, j) (((i) > (j)) ? (i) : (j))

#define NB_j 4 // nombre de joueurs
#define NB_b 4 // nombre de bombes

#define TILE_SIZE 64
#define WINDOW_WIDTH TILE_SIZE*13
#define WINDOW_HEIGHT TILE_SIZE*11

struct plateau{
    int** tabl;
    int lignes;
    int collonnes;
};


struct plateau Init_map(int n, int m) {

    // initialisation de la carte
    int **carte = malloc (n * sizeof (int*)) ; // création de la carte
    for (int i = 0; i < n; i++){ carte[i] = malloc (m * sizeof (int)) ;}

    for(int i = 0; i<n; i++){ // bordures de la carte
        carte[i][0] = -1;
        carte[i][m-1] = -1;
    }
    for(int j = 0; j<m; j++){
        carte[0][j] = -1;
        carte[n-1][j] = -1;
    }
    for (int i = 1; i<n-1; i++){ // murs cassables
        for (int j = 1; j < m-1; j++){
            carte[i][j] = 1;
        }
    }
    for(int i2 = 2; i2+2<n; i2+=2){ // ajout des murs incassables
        for(int j2 = 2; j2+2<m; j2+=2){
            carte[i2][j2] = -1;
        }
    }
    carte[1][1] = 0; carte[2][1] = 0; carte[1][2] = 0; // blocs pour le départ des joueurs
    carte[1][m-2] = 0; carte[2][m-2] = 0; carte[1][m-3] = 0;
    carte[n-2][1] = 0; carte[n-3][1] = 0; carte[n-2][2] = 0;
    carte[n-2][m-2] = 0; carte[n-3][m-2] = 0; carte[n-2][m-3] = 0;

    struct plateau pl = {carte, n, m};

    return pl;
}


// Structure pour stocker les informations sur une image
typedef struct {
    SDL_Texture* texture;   // Texture SDL pour l'image
    int width;              // Largeur de l'image en pixels
    int height;             // Hauteur de l'image en pixels
} Image;


// Charge une image à partir d'un fichier et retourne une structure Image
Image load_image(const char* filename, SDL_Renderer* renderer) {
    Image image;
    char filepath[100] = "textures/";
    strcat(filepath, filename);
    SDL_Surface* surface = IMG_Load(filepath);
    if (!surface) {
        fprintf(stderr, "Erreur lors du chargement de l'image %s : %s\n", filepath, IMG_GetError());
        exit(EXIT_FAILURE);
    }
    image.texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!image.texture) {
        fprintf(stderr, "Erreur lors de la création de la texture pour l'image %s : %s\n", filepath, SDL_GetError());
        exit(EXIT_FAILURE);
    }
    image.width = surface->w;
    image.height = surface->h;
    SDL_FreeSurface(surface);
    return image;
}


int main(int argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erreur d'initialisation de la SDL : %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window* window = SDL_CreateWindow("Pavage d'images", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Erreur de création de la fenêtre : %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        printf("Erreur de création du rendu : %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Chargement des images
    Image images[3];    // On charge 3 images pour notre exemple
    images[0] = load_image("obsi64.png", renderer);
    images[1] = load_image("grass64.png", renderer);
    images[2] = load_image("bois64.png", renderer);

    // Matrice de choix d'images
    int** matrix = Init_map(WINDOW_HEIGHT / TILE_SIZE, WINDOW_WIDTH / TILE_SIZE).tabl;
    
    // Boucle principale
    SDL_Event event;
    int quit = 0;
    while (!quit) {
        // Gestion des événements
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            }
        }

        // Effacement de l'écran
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Affichage du pavage d'images
                for (int i = 0; i < WINDOW_HEIGHT / TILE_SIZE; i++) {
            for (int j = 0; j < WINDOW_WIDTH / TILE_SIZE; j++) {
                Image image = images[matrix[i][j]+1];
                SDL_Rect dst_rect = {j * TILE_SIZE, i * TILE_SIZE, image.width, image.height};
                SDL_RenderCopy(renderer, image.texture, NULL, &dst_rect);
            }
        }

        // Affichage de la fenêtre
        SDL_RenderPresent(renderer);
    }

    // Libération des ressources
    for (int i = 0; i < 3; i++) {
        SDL_DestroyTexture(images[i].texture);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
