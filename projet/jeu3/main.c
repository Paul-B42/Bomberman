#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
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

// pourcentage de chance d'obtenir un bonus
#define P_bonus 30

#define P_B1 25
#define P_B2 25
#define P_B3 25
#define P_B4 25

#define P_blocs 5 // poucentage de bloc destructible (entier compris entre 0 et 100)

struct plateau{
    int** tabl;
    int lignes;
    int collonnes;
};

struct player{
    bool etat; // vivant ou mort
    char* nom; // nom du joueur
    int vie; // entier compris entre 0 et 10
    int x; int y; // position du joueur
    char direction; // direction du joueur nord, sud, est, ouest
    int speed; // vitesse du personnage
    int* inventory; // inventaire des objets du joueur
    int rank;
};

struct bombe{
    int L;
    int C;
};

struct point{
    int i;
    int j;
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
            a = rand()%99;
            if(a < P_blocs){carte[i][j] = 1;}else{carte[i][j] = 0;}
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
        case 'N' : if ((carte[i+1][j] == 0 || carte[i+1][j] > 5) && !same(i+1, j, ID, Joueurs)){return true;}; break;
        case 'S' : if ((carte[i-1][j] == 0 || carte[i-1][j] > 5) && !same(i-1, j, ID, Joueurs)){return true;}; break;
        case 'E' : if ((carte[i][j+1] == 0 || carte[i][j+1] > 5) && !same(i, j+1, ID, Joueurs)){return true;}; break;
        case 'O' : if ((carte[i][j-1] == 0 || carte[i][j-1] > 5) && !same(i, j-1, ID, Joueurs)){return true;}; break;
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
        switch(carte[Joueurs[ID]->y][Joueurs[ID]->x]){
            case 6 : Joueurs[ID]->inventory[0] += 5; carte[Joueurs[ID]->y][Joueurs[ID]->x] = 0; break;
            case 7 : Joueurs[ID]->inventory[1] += 5; carte[Joueurs[ID]->y][Joueurs[ID]->x] = 0; break;
            case 8 : Joueurs[ID]->inventory[2] += 5; carte[Joueurs[ID]->y][Joueurs[ID]->x] = 0; break;
            case 9 : Joueurs[ID]->inventory[3] += 5; carte[Joueurs[ID]->y][Joueurs[ID]->x] = 0; break;
            case 10 : Joueurs[ID]->vie ++; carte[Joueurs[ID]->y][Joueurs[ID]->x] = 0; break;
        }
    }else{
        Joueurs[ID]->direction = dir;
    }
}



void poser_item(struct plateau* plat, int x, int y, int ID_item){
    if (plat->tabl[y][x] == 0){
        plat->tabl[y][x] = ID_item;
    } 
}



int break_bloc(struct plateau*  plat, struct player* Joueurs[NB_j], int i, int j){ 
        int A = 0;
        int bonus;

        if (plat->tabl[i][j] == 1){ // mur cassable + bonus
            plat->tabl[i][j] = 0; 
            bonus = rand()%99;
            if (bonus < P_bonus){
                bonus = rand()%4;
                switch (bonus){
                    case 0 : plat->tabl[i][j] = 6; break;
                    case 1 : plat->tabl[i][j] = 7; break;
                    case 2 : plat->tabl[i][j] = 8; break;
                    case 3 : plat->tabl[i][j] = 9; break;
                    case 4 : plat->tabl[i][j] = 10; break;
                }
            }


        } else if (plat->tabl[i][j] > 1 && plat->tabl[i][j] < 6){ // bombes 
            A = plat->tabl[i][j];
            plat->tabl[i][j] = 0;

        } else if (plat->tabl[i][j] > 5){
            plat->tabl[i][j] = 0;
        }
        
        for(int k = 0; k<NB_j; k++){ 
            if (Joueurs[k]->y == i && Joueurs[k]->x ==j){ // joueurs
                Joueurs[k]->vie -= 1;
                if (Joueurs[k]->vie == 0 && Joueurs[k]->etat){
                    Joueurs[k]->etat = false;
                    Joueurs[k]->x = 0;
                    Joueurs[k]->y = 0;
                    Joueurs[k]->direction = 'N';
                    for(int l = 0; l<NB_j; l++){
                        if(l != k && Joueurs[l]->etat){Joueurs[l]->rank--;}
                    }
                    
                }
            }
        }
        return A; // prévient si il détruit une bombe
}



void explosion(struct plateau*  plat, struct player* Joueurs[NB_j], int i, int j, struct bombe Bomb[NB_b], Uint32** boom){
    int A = break_bloc(plat, Joueurs, i,j); // ID_bombe = valeur sur la carte -2 
    
    if (A > 1 && A < 6){
    int l = Bomb[A-2].L;
    int c = Bomb[A-2].C;
    int k;
    boom[i][j] = SDL_GetTicks();

    // direction Nord
    k = 1; 
    while(k <= c && plat->tabl[i+k][j] != -1){
        A = break_bloc(plat, Joueurs, i+k, j); 
        boom[i+k][j] = SDL_GetTicks();
        if (A != 0){
            plat->tabl[i+k][j] = A;
            explosion(plat, Joueurs, i+k, j, Bomb, boom); 
            A = 0; 
        }
        k++;
    }
    // direction Sud
    k = 1; 
    while(k <= c && plat->tabl[i-k][j] != -1){
        A = break_bloc(plat, Joueurs, i-k, j); 
        boom[i-k][j] = SDL_GetTicks();
        if (A != 0){
            plat->tabl[i-k][j] = A;
            explosion(plat, Joueurs, i-k, j, Bomb, boom);
            A = 0; 
        }
        k++;
    }
    // direction Est
    k = 1; 
    while(k <= l && plat->tabl[i][j+k] != -1){
        A = break_bloc(plat, Joueurs, i, j+k); 
        boom[i][j+k] = SDL_GetTicks();
        if (A != 0){
            plat->tabl[i][j+k] = A;
            explosion(plat, Joueurs, i, j+k, Bomb, boom);
            A = 0; 
        }
        k++;
    }
    // direction Ouest
    k = 1; 
    while(k <= l && plat->tabl[i][j-k] != -1){
        A = break_bloc(plat, Joueurs, i, j-k); 
        boom[i][j-k] = SDL_GetTicks();
        if (A != 0){
            plat->tabl[i][j-k] = A;
            explosion(plat, Joueurs, i, j-k, Bomb, boom);
            A = 0; 
        }
        k++;
    }
    }
}
/*******************************************************/

// initialisation
void Init_Joueurs(struct player* Joueurs[NB_j]){
    for (int k = 0; k < NB_j; k++){
        Joueurs[k]->etat = true;
        Joueurs[k]->vie = 100;
        Joueurs[k]->direction = 'N';
        Joueurs[k]->rank = 4;
        for (int i = 0; i<4; i++){Joueurs[k]->inventory[i] = 10;}
    }
    Joueurs[0]->x = 1; Joueurs[0]->y = 1;
    Joueurs[1]->x = WINDOW_WIDTH / TILE_SIZE -2; Joueurs[1]->y = WINDOW_HEIGHT / TILE_SIZE -2;
    Joueurs[2]->x = 1; Joueurs[2]->y = WINDOW_HEIGHT / TILE_SIZE -2;
    Joueurs[3]->x = WINDOW_WIDTH / TILE_SIZE -2; Joueurs[3]->y = 1;
}


// fonction pour écrire un texte sur un renderer
void write(char* texte, TTF_Font* font, SDL_Color color, SDL_Renderer* renderer, int x, int y){
    char text[10];
    sprintf(text, "%s", texte);
    SDL_Surface* Surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* Texture = SDL_CreateTextureFromSurface(renderer, Surface);
    SDL_Rect Rect = {x, y, Surface->w, Surface->h};
    SDL_RenderCopy(renderer, Texture, NULL, &Rect);
    SDL_FreeSurface(Surface);
    SDL_DestroyTexture(Texture);
}


// fonction pour écrire des chiffres sur un renderer
void write_number(int num, TTF_Font* font, SDL_Color color, SDL_Renderer* renderer, int x, int y){
    char text[10];
    sprintf(text, "%d", num);
    SDL_Surface* Surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* Texture = SDL_CreateTextureFromSurface(renderer, Surface);
    SDL_Rect Rect = {x, y, Surface->w, Surface->h};
    SDL_RenderCopy(renderer, Texture, NULL, &Rect);
    SDL_FreeSurface(Surface);
    SDL_DestroyTexture(Texture);
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

    // Chargement des images blocs
    Image images[13];    
    images[0] = load_image("stone64.png", renderer);
    images[1] = load_image("grass64.png", renderer);
    images[2] = load_image("bois64.png", renderer);
    images[3] = load_image("tnt64.png", renderer);
    images[4] = load_image("tnt_bleu-3.jpg", renderer);
    images[5] = load_image("tnt_verte-3.jpg", renderer);
    images[6] = load_image("tnt_violet-3.jpg", renderer);
    images[7] = load_image("bonus.png", renderer);
    images[8] = load_image("bonus.png", renderer);
    images[9] = load_image("bonus.png", renderer);
    images[10] = load_image("bonus.png", renderer);
    images[11] = load_image("bonus.png", renderer);
    images[12] = load_image("boom1.png", renderer);

    // Chargement des images joueurs
char* skins[NB_j] = {"Mario", "Bowser", "DK", "Yoshi"};
char chemin[50];
Image player_images[NB_j][4];

for (int k = 0; k < NB_j; k++) {
    strcpy(chemin, "skins/");
    strcat(chemin, skins[k]);

    // Chargement de l'image direction Nord
    strcat(chemin, "/N.png");
    player_images[k][0] = load_image(chemin, renderer);

    // Chargement de l'image direction Sud
    strcpy(chemin, "skins/");
    strcat(chemin, skins[k]);
    strcat(chemin, "/S.png");
    player_images[k][1] = load_image(chemin, renderer);

    // Chargement de l'image direction Est
    strcpy(chemin, "skins/");
    strcat(chemin, skins[k]);
    strcat(chemin, "/E.png");
    player_images[k][2] = load_image(chemin, renderer);

    // Chargement de l'image direction Ouest
    strcpy(chemin, "skins/");
    strcat(chemin, skins[k]);
    strcat(chemin, "/O.png");
    player_images[k][3] = load_image(chemin, renderer);
}

    

    // Matrice de choix d'images
    struct plateau map = Init_map(WINDOW_HEIGHT / TILE_SIZE, WINDOW_WIDTH / TILE_SIZE);
    int** matrix = map.tabl;
    
    // initialisation des joueurs
    struct player* Joueurs[NB_j];
    int inv1[] = {10, 10, 10, 10}; 
    int inv2[] = {10, 10, 10, 10}; 
    int inv3[] = {10, 10, 10, 10}; 
    int inv4[] = {10, 10, 10, 10}; 
    struct player J1 = {true, "Romain", 100, 1, 1, 'N', 1, inv1,4}; 
    struct player J2 = {true, "Christophe", 100, WINDOW_WIDTH / TILE_SIZE -2, WINDOW_HEIGHT / TILE_SIZE -2, 'N', 1, inv2,4};
    struct player J3 = {true, "Orso", 100, 1, WINDOW_HEIGHT / TILE_SIZE -2, 'N', 1, inv3,4};
    struct player J4 = {true, "Paul", 100, WINDOW_WIDTH / TILE_SIZE -2, 1, 'N', 1, inv4,4};
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

    // tableau des blocs cassé
    Uint32 **boom = malloc (WINDOW_HEIGHT / TILE_SIZE * sizeof (Uint32*)) ; // création de la carte
    for (int i = 0; i < WINDOW_HEIGHT / TILE_SIZE; i++){ boom[i] = malloc (WINDOW_WIDTH / TILE_SIZE * sizeof (Uint32)) ;}
    for (int i = 0; i < WINDOW_HEIGHT / TILE_SIZE; i++){
        for (int j = 0; j < WINDOW_WIDTH / TILE_SIZE; j++){
            boom[i][j]  = 0;
        }
    }

    // tableau des bombes
    Uint32 **explose = malloc (WINDOW_HEIGHT / TILE_SIZE * sizeof (Uint32*)) ; // création de la carte
    for (int i = 0; i < WINDOW_HEIGHT / TILE_SIZE; i++){ explose[i] = malloc (WINDOW_WIDTH / TILE_SIZE * sizeof (Uint32)) ;}
    for (int i = 0; i < WINDOW_HEIGHT / TILE_SIZE; i++){
        for (int j = 0; j < WINDOW_WIDTH / TILE_SIZE; j++){
            explose[i][j]  = 0;
        }
    }

    // texte - inventaire
    TTF_Init();
    TTF_Font* font = TTF_OpenFont("polices/Georgia.ttf", 48);
    SDL_Color color = {255, 255, 255}; 

    // timer
    Uint32 delay = 3000; // Temps de délai en millisecondes (3 secondes)
    Uint32 wait = 1000; // temps d'exposition de l'explosion


    // Boucle principale
    SDL_Event event;
    Uint32 end = 0;
    bool fin = false;
    int quit = 0;
    int ID = 0;
    int a = 0;
    TTF_Init();

    Image image = load_image("loading.png", renderer);
    SDL_Rect dst_rect = {0, 0, image.width, image.height};
    SDL_RenderCopy(renderer, image.texture, NULL, &dst_rect);
    SDL_RenderPresent(renderer);

    SDL_Delay(wait+delay);

    while (!quit) {

        // Gestion des événements
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            } else if (event.type == SDL_KEYDOWN && Joueurs[ID]->vie > 0) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP: move(matrix, ID, 'S', Joueurs); break;
                    case SDLK_DOWN: move(matrix, ID, 'N', Joueurs); break;
                    case SDLK_LEFT: move(matrix, ID, 'O', Joueurs); break;
                    case SDLK_RIGHT: move(matrix, ID, 'E', Joueurs); break;

                    case SDLK_a : 
                        if (Joueurs[ID]->inventory[0] > 0){
                            poser_item(&map, Joueurs[ID]->x, Joueurs[ID]->y, 2);
                            explose[Joueurs[ID]->y][Joueurs[ID]->x] = SDL_GetTicks();
                            Joueurs[ID]->inventory[0] --;
                        }
                        break;
                    case SDLK_z : 
                        if (Joueurs[ID]->inventory[1] > 0){
                            poser_item(&map, Joueurs[ID]->x, Joueurs[ID]->y, 3);
                            explose[Joueurs[ID]->y][Joueurs[ID]->x] = SDL_GetTicks();
                            Joueurs[ID]->inventory[1] --;
                        }
                        break;
                    case SDLK_e : 
                        if (Joueurs[ID]->inventory[2] > 0){
                            poser_item(&map, Joueurs[ID]->x, Joueurs[ID]->y, 4);
                            explose[Joueurs[ID]->y][Joueurs[ID]->x] = SDL_GetTicks();
                            Joueurs[ID]->inventory[2] --;
                        }
                        break;
                    case SDLK_r : 
                        if (Joueurs[ID]->inventory[3] > 0){
                            poser_item(&map, Joueurs[ID]->x, Joueurs[ID]->y, 5);
                            explose[Joueurs[ID]->y][Joueurs[ID]->x] = SDL_GetTicks();
                            Joueurs[ID]->inventory[3] --;
                        }
                        break;
                }
            }
        }

        // gérer les explosions
        for (int i = 1; i < WINDOW_HEIGHT / TILE_SIZE -1; i++) {
            for (int j = 1; j < WINDOW_WIDTH / TILE_SIZE -1; j++) {
                if (abs(SDL_GetTicks() - explose[i][j] - delay) < 20 ){
                    explosion(&map, Joueurs, i, j, Bomb, boom);
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
                // Affichage des explosions
                if(SDL_GetTicks() - boom[i][j] < wait){
                    break_bloc(&map, Joueurs, i, j);
                    Image image = images[12];
                    SDL_Rect dst_rect = {j * TILE_SIZE, i * TILE_SIZE, image.width, image.height};
                    SDL_RenderCopy(renderer, image.texture, NULL, &dst_rect);
                }
            }
        }

        // Affichage du joueur à sa position actuelle
        for (int k = 0; k < NB_j; k++) {
            SDL_Rect tempRect;
            Image playerImage;
            SDL_Rect* playerRect = &tempRect;
            
            switch (Joueurs[k]->direction) {
                case 'S':
                    playerImage = player_images[k][0];
                    break;
                case 'N':
                    playerImage = player_images[k][1];
                    break;
                case 'E':
                    playerImage = player_images[k][2];
                    break;
                case 'O':
                    playerImage = player_images[k][3];
                    break;
            }
            
            playerRect->w = playerImage.width;
            playerRect->h = playerImage.height;

            if (!Joueurs[k]->etat) {
                playerRect->x = Joueurs[k]->rank * TILE_SIZE;
                playerRect->y = 0;
            } else {
                playerRect->x = Joueurs[k]->x * TILE_SIZE;
                playerRect->y = Joueurs[k]->y * TILE_SIZE;
            }

            SDL_RenderCopy(renderer, playerImage.texture, NULL, playerRect);
        }

        // Affichage de la vie du joueur
        write_number(Joueurs[ID]->vie, font, color, renderer, 0, 0);

        // Affichage du nom du joueur
        write(Joueurs[ID]->nom, font, color, renderer, TILE_SIZE, WINDOW_HEIGHT - TILE_SIZE);

        // Affichage de l'inventaire du joueur
        for (int i = 0; i < 4; i++) {
            write_number(Joueurs[ID]->inventory[i], font, color, renderer, 0, (i+2)*TILE_SIZE);
        }

        // fin du jeu
        if (!fin){
            a = 0;
            for (int k = 0; k<NB_j; k++){if(Joueurs[k]->etat){a++;}}
            if (a<2){fin = true; end = SDL_GetTicks();}
        }
        if (SDL_GetTicks() - end > delay && fin){
            Image image = load_image("leaderbord.png", renderer);
            SDL_Rect dst_rect = {0, 0, image.width, image.height};
            SDL_RenderCopy(renderer, image.texture, NULL, &dst_rect);
            for (int k = 0; k < NB_j; k++){
                strcpy(chemin, "skins/"); 
                strcat(chemin, skins[k]);
                strcat(chemin, "/Big.png");
                Image image = load_image(chemin, renderer);
                SDL_Rect dst_rect = {290, 195+(Joueurs[k]->rank-1)*150, image.width, image.height};
                SDL_RenderCopy(renderer, image.texture, NULL, &dst_rect); 
                write(Joueurs[k]->nom, font, color, renderer, 450, 195+(Joueurs[k]->rank-1)*150+40);
            }
            SDL_RenderPresent(renderer);

            while (SDL_PollEvent(&event) && fin){
                if (event.type == SDL_QUIT) {
                    quit = 1;
                } else if (event.type == SDL_KEYDOWN) {
                    if (event.key.keysym.sym == SDLK_SPACE){
                        fin = false;
                        a = 0;
                        map = Init_map(WINDOW_HEIGHT / TILE_SIZE, WINDOW_WIDTH / TILE_SIZE);
                        matrix = map.tabl;
                        Init_Joueurs(Joueurs);
                    }
                }
            }
        }
        

        // Affichage de la fenêtre
        SDL_RenderPresent(renderer);
    }
    

    // Libération des ressources
    for (int i = 0; i < WINDOW_HEIGHT / TILE_SIZE; i++){free(matrix[i]); free(boom[i]); free(explose[i]);}
    free (matrix) ; free(boom); free(explose);
    for (int i = 0; i < 13; i++) { SDL_DestroyTexture(images[i].texture);}
    for (int i = 0; i < NB_j; i++) {for (int j = 0; j < 4; j++){SDL_DestroyTexture(player_images[i][j].texture);}}
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
