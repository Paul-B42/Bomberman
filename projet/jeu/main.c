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
#define WINDOW_WIDTH TILE_SIZE*17
#define WINDOW_HEIGHT TILE_SIZE*13

struct plateau{
    int** tabl;
    int lignes;
    int collonnes;
};

struct player{
    char* nom; // nom du joueur
    int vie; // entier compris entre 0 et 10
    int x; int y; // position du joueur
    char direction; // direction du joueur nord, sud, est, ouest
    int speed; // vitesse du personnage
    int* inventory; // inventaire des objets du joueur
};

struct bombe{
    int L;
    int C;
};

struct plateau Init_map(int n, int m) {

    srand(time(NULL));
    int a;

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
            a = rand()%5;
            if(a>0){carte[i][j] = 1;}else{carte[i][j] = 0;}
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

void animation(int i, int j){

}

// fonctions principales
/*******************************************************/
bool same(int i, int j, int ID, struct player* Joueurs[NB_j]){
    for (int k = 0; k < NB_j; k++){
        if (k != ID){if(Joueurs[k]->y == i && Joueurs[k]->x == j){
            return true;
        }}
    }
    return false;
}


bool is_free(int** carte, char dir, int ID, struct player* Joueurs[NB_j]){
    int i = Joueurs[ID]->y;
    int j = Joueurs[ID]->x;
    switch (dir){
        case 'N' : if (carte[i+1][j] == 0 && !same(i+1, j, ID, Joueurs)){return true;}; break;
        case 'S' : if (carte[i-1][j] == 0 && !same(i-1, j, ID, Joueurs)){return true;}; break;
        case 'E' : if (carte[i][j+1] == 0 && !same(i, j+1, ID, Joueurs)){return true;}; break;
        case 'O' : if (carte[i][j-1] == 0 && !same(i, j-1, ID, Joueurs)){return true;}; break;
    }
    return false;
}



void move(int** carte, int ID, char dir, struct player* Joueurs[NB_j]){
    if (is_free(carte, dir, ID, Joueurs)){
        Joueurs[ID]->direction = dir;
        switch (dir){
            case 'N' : Joueurs[ID]->y += 1; break;
            case 'S' : Joueurs[ID]->y -= 1; break;
            case 'E' : Joueurs[ID]->x += 1; break;
            case 'O' : Joueurs[ID]->x -= 1; break;
        }
    }
}



void poser_item(struct plateau* plat, int x, int y, int ID_item){
    plat->tabl[y][x] = ID_item;
}



int break_bloc(struct plateau*  plat, struct player* Joueurs[NB_j], int i, int j){ 
        int A = 0;

        if (plat->tabl[i][j] == 1){ // mur cassable
            plat->tabl[i][j] = 0; 
        } else if (plat->tabl[i][j] > 1){ // bombes 
            A = plat->tabl[i][j];
            plat->tabl[i][j] = 0;
        } 
        
        for(int k = 0; k<NB_j; k++){ 
            if (Joueurs[k]->y == i && Joueurs[k]->x ==j){ // joueurs
                Joueurs[k]->vie -= 1;
            }
        }
        return A; // prévient si il détruit une bombe
}



void explosion(struct plateau*  plat, struct player* Joueurs[NB_j], int i, int j, struct bombe Bomb[NB_b]){
    int A = break_bloc(plat, Joueurs, i,j); // ID_bombe = valeur sur la carte -2 
    int l = Bomb[A-2].L;
    int c = Bomb[A-2].C;
    int k;

    // direction Nord
    k = 1; 
    while(k <= c && plat->tabl[i+k][j] != -1){
        A = break_bloc(plat, Joueurs, i+k, j); 
        if (A != 0){
            plat->tabl[i+k][j] = A;
            explosion(plat, Joueurs, i+k, j, Bomb); 
            A = 0; 
        }
        k++;
    }
    // direction Sud
    k = 1; 
    while(k <= c && plat->tabl[i-k][j] != -1){
        A = break_bloc(plat, Joueurs, i-k, j); 
        if (A != 0){
            plat->tabl[i-k][j] = A;
            explosion(plat, Joueurs, i-k, j, Bomb);
            A = 0; 
        }
        k++;
    }
    // direction Est
    k = 1; 
    while(k <= l && plat->tabl[i][j+k] != -1){
        A = break_bloc(plat, Joueurs, i, j+k); 
        if (A != 0){
            plat->tabl[i][j+k] = A;
            explosion(plat, Joueurs, i, j+k, Bomb);
            A = 0; 
        }
        k++;
    }
    // direction Ouest
    k = 1; 
    while(k <= l && plat->tabl[i][j-k] != -1){
        A = break_bloc(plat, Joueurs, i, j-k); 
        if (A != 0){
            plat->tabl[i][j-k] = A;
            explosion(plat, Joueurs, i, j-k, Bomb);
            A = 0; 
        }
        k++;
    }
}
/*******************************************************/


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
    if (SDL_Init(SDL_INIT_TIMER) != 0) {
        printf("Erreur d'initialisation de la SDL : %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

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
    Image images[11];    // On charge 3 images 
    images[0] = load_image("obsi64.png", renderer);
    images[1] = load_image("grass64.png", renderer);
    images[2] = load_image("bois64.png", renderer);
    images[3] = load_image("tnt64.png", renderer);
    images[4] = load_image("tnt64.png", renderer);
    images[5] = load_image("tnt64.png", renderer);
    images[6] = load_image("tnt64.png", renderer);
    images[7] = load_image("J1_64.png", renderer);
    images[8] = load_image("J2_64.png", renderer);
    images[9] = load_image("J3_64.png", renderer);
    images[10] = load_image("J4_64.png", renderer);
    

    // Matrice de choix d'images
    struct plateau map = Init_map(WINDOW_HEIGHT / TILE_SIZE, WINDOW_WIDTH / TILE_SIZE);
    int** matrix = map.tabl;
    
    // initialisation des joueurs
    struct player* Joueurs[NB_j];
    struct player J1 = {"Romain", 3, 1, 1, 'N', 1, NULL}; // noms avec des nscanf
    struct player J2 = {"Christophe", 3, WINDOW_WIDTH / TILE_SIZE -2, WINDOW_HEIGHT / TILE_SIZE -2, 'S', 1, NULL};
    struct player J3 = {"Orso", 3, 1, WINDOW_HEIGHT / TILE_SIZE -2, 'E', 1, NULL};
    struct player J4 = {"Paul", 3, WINDOW_WIDTH / TILE_SIZE -2, 1, 'O', 1, NULL};
    Joueurs[0] = &J1;
    Joueurs[1] = &J2;
    Joueurs[2] = &J3;
    Joueurs[3] = &J4;

    // définition des bombes
    struct bombe Bomb[NB_b];
    struct bombe B1 = {1,1}; // croix (5) // 2
    struct bombe B2 = {2,2}; // croix (9) // 3
    struct bombe B3 = {3,0}; // ligne (7) // 4
    struct bombe B4 = {0,3}; // collonne (7) // 5
    Bomb[0] = B1;
    Bomb[1] = B2;
    Bomb[2] = B3;
    Bomb[3] = B4;

    // timer
    SDL_TimerID timerID;
    Uint32 delay = 3000; // Temps de délai en millisecondes (3 secondes)

    typedef struct {
    int i;
    int j;
    } TimerParams;

    // Fonction appelée lorsque le délai du timer est atteint
    Uint32 timerCallback(Uint32 interval, void* param) {
        TimerParams* params = (TimerParams*)param;
        explosion(&map, Joueurs, params->i, params->j, Bomb);
    return 0; // Valeur de retour 0 pour arrêter le timer
    }
    TimerParams* params = (TimerParams*)malloc(sizeof(TimerParams));

    // Boucle principale
    SDL_Event event;
    int quit = 0;
    int ID = 0;


    while (!quit) {
        // Gestion des événements
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP: move(matrix, ID, 'S', Joueurs); break;
                    case SDLK_DOWN: move(matrix, ID, 'N', Joueurs); break;
                    case SDLK_LEFT: move(matrix, ID, 'O', Joueurs); break;
                    case SDLK_RIGHT: move(matrix, ID, 'E', Joueurs); break;

                    case SDLK_a : poser_item(&map, Joueurs[ID]->x, Joueurs[ID]->y, 2); 
                        params->i = Joueurs[ID]->y;
                        params->j = Joueurs[ID]->x;
                        timerID = SDL_AddTimer(delay, timerCallback, params);
                        if (timerID == 0) {
                            printf("Erreur lors de la création du timer : %s\n", SDL_GetError());
                            SDL_Quit();
                            return EXIT_FAILURE;
                        }
                        break;
                    case SDLK_z : poser_item(&map, Joueurs[ID]->x, Joueurs[ID]->y, 3); 
                        params->i = Joueurs[ID]->y;
                        params->j = Joueurs[ID]->x;
                        timerID = SDL_AddTimer(delay, timerCallback, params);
                        break;
                    case SDLK_e : poser_item(&map, Joueurs[ID]->x, Joueurs[ID]->y, 4); 
                        params->i = Joueurs[ID]->y;
                        params->j = Joueurs[ID]->x;
                        timerID = SDL_AddTimer(delay, timerCallback, params);
                        break;
                    case SDLK_r : poser_item(&map, Joueurs[ID]->x, Joueurs[ID]->y, 5); 
                        params->i = Joueurs[ID]->y;
                        params->j = Joueurs[ID]->x;
                        timerID = SDL_AddTimer(delay, timerCallback, params);
                        break;
                }
            }
        }

        // Effacement de l'écran
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // Affichage du pavage d'images
        for (int i = 0; i < WINDOW_HEIGHT / TILE_SIZE; i++) {
            for (int j = 0; j < WINDOW_WIDTH / TILE_SIZE; j++) {
                Image image = images[matrix[i][j] + 1];
                SDL_Rect dst_rect = {j * TILE_SIZE, i * TILE_SIZE, image.width, image.height};
                SDL_RenderCopy(renderer, image.texture, NULL, &dst_rect);
            }
        }

        // Affichage du joueur à sa position actuelle
        for (int k = 0; k < NB_j; k++){
            Image playerImage = images[7+k];  // Image pour le joueur
            SDL_Rect playerRect = {Joueurs[k]->x * TILE_SIZE, Joueurs[k]->y * TILE_SIZE, playerImage.width, playerImage.height};
            SDL_RenderCopy(renderer, playerImage.texture, NULL, &playerRect);
        }

        // Affichage de la fenêtre
        SDL_RenderPresent(renderer);
    }


    // Libération des ressources
    free(params); 
    SDL_RemoveTimer(timerID);
    for (int i = 0; i < 11; i++) {
        SDL_DestroyTexture(images[i].texture);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
