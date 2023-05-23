#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
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
#define P_blocs 50 // poucentage de bloc destructible (entier compris entre 0 et 100)

#define disp 200 // temps en ms de propagation de l'explosion
#define pas 8;

char* PSEUDOS[NB_j] = {"Romain", "Christophe", "Orso", "Paul"};
char* SKINS[NB_j] = {"Kamec", "Peach", "Mario", "Bowser"};

int POINTS[NB_j] = {5, 4, 3, 2};
Uint32 end = -4000;



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
    int score;
    int frame;
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



bool move(int** carte, int ID, char dir, struct player* Joueurs[NB_j]){
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
            case 11 : Joueurs[ID]->frame = 1; carte[Joueurs[ID]->y][Joueurs[ID]->x] = 0; break;
        }
        return true;
    }else{
        Joueurs[ID]->direction = dir;
        return false;
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
                bonus = rand()%9;
                switch (bonus){
                    case 0 : plat->tabl[i][j] = 6; break;
                    case 1 : plat->tabl[i][j] = 7; break;
                    case 2 : plat->tabl[i][j] = 8; break;
                    case 3 : plat->tabl[i][j] = 9; break;
                    case 4 : plat->tabl[i][j] = 10; break;
                    default : plat->tabl[i][j] = 11; break;
                }
            }


        } else if (plat->tabl[i][j] > 1 && plat->tabl[i][j] < 6){ // bombes 
            A = plat->tabl[i][j];
            plat->tabl[i][j] = 0;

        } else if (plat->tabl[i][j] > 5){
            plat->tabl[i][j] = 0;
        }
        
        for(int k = 0; k<NB_j; k++){ 
            if (Joueurs[k]->y == i && Joueurs[k]->x ==j && Joueurs[k]->etat){ // joueurs
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
    boom[i][j] = SDL_GetTicks()-end;

    // direction Nord
    k = 1; 
    while(k <= c && plat->tabl[i+k][j] != -1){
        A = break_bloc(plat, Joueurs, i+k, j); 
        boom[i+k][j] = SDL_GetTicks()-end + k*disp;
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
        boom[i-k][j] = SDL_GetTicks()-end + k*disp;
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
        boom[i][j+k] = SDL_GetTicks()-end + k*disp;
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
        boom[i][j-k] = SDL_GetTicks()-end + k*disp;
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
        Joueurs[k]->vie = 3;
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
void write(char text[10], TTF_Font* font, SDL_Color color, SDL_Renderer* renderer, int x, int y){
    SDL_Surface* Surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* Texture = SDL_CreateTextureFromSurface(renderer, Surface);
    SDL_Rect Rect = {x, y, Surface->w, Surface->h};
    SDL_RenderCopy(renderer, Texture, NULL, &Rect);
    SDL_FreeSurface(Surface);
    SDL_DestroyTexture(Texture);
}


// classement des joueurs en fonction de leurs scores
int* classement(struct player* Joueurs[NB_j]) {
    static int cl[NB_j] = {0, 1, 2, 3};
    
    // Trie les indices en fonction des scores
    for (int i = 0; i < NB_j - 1; i++) {
        for (int j = i + 1; j < NB_j; j++) {
            if (Joueurs[cl[j]]->score > Joueurs[cl[i]]->score) {
                int temp = cl[i];
                cl[i] = cl[j];
                cl[j] = temp;
            }
        }
    }
    
    return cl;
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




int main() {

    // Initialisation de SDL
    if (SDL_Init(/*SDL_INIT_AUDIO |*/ SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0) {
        printf("Erreur lors de l'initialisation de SDL : %s\n", SDL_GetError());
        return 1;
    }

    // // Initialisation de SDL Mixer
    // if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
    //     printf("Erreur lors de l'initialisation de SDL Mixer : %s\n", Mix_GetError());
    //     return 1;
    // }

    // // Chargement du fichier audio
    // Mix_Music* music = Mix_LoadMUS("sons/son_rickroll.mp3");

    SDL_Window* window = SDL_CreateWindow("Bomberman", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
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
    //SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    // Chargement des images blocs
    Image images[17];    
    images[0] = load_image("incassable.png", renderer);
    images[1] = load_image("vide.png", renderer);
    images[2] = load_image("cassable.png", renderer);
    images[3] = load_image("bombe1.png", renderer);
    images[4] = load_image("bombe2.png", renderer);
    images[5] = load_image("bombe3.png", renderer);
    images[6] = load_image("bombe4.png", renderer);
    images[7] = load_image("bonus.png", renderer);
    images[8] = load_image("bonus.png", renderer);
    images[9] = load_image("bonus.png", renderer);
    images[10] = load_image("bonus.png", renderer);
    images[11] = load_image("bonus.png", renderer);
    images[12] = load_image("bonus.png", renderer);
    images[13] = load_image("boom.png", renderer);
    images[14] = load_image("rickroll_1.png", renderer);
    images[15] = load_image("rickroll_2.png", renderer);
    images[16] = load_image("rickroll_3.png", renderer);

    // Chargement des images joueurs
    char chemin[50];
    Image player_images[NB_j][5];

    for (int k = 0; k < NB_j; k++) {
        strcpy(chemin, "skins/");
        strcat(chemin, SKINS[k]);
        strcat(chemin, "/N.png");
        player_images[k][0] = load_image(chemin, renderer);
        strcpy(chemin, "skins/");
        strcat(chemin, SKINS[k]);
        strcat(chemin, "/S.png");
        player_images[k][1] = load_image(chemin, renderer);
        strcpy(chemin, "skins/");
        strcat(chemin, SKINS[k]);
        strcat(chemin, "/E.png");
        player_images[k][2] = load_image(chemin, renderer);
        strcpy(chemin, "skins/");
        strcat(chemin, SKINS[k]);
        strcat(chemin, "/O.png");
        player_images[k][3] = load_image(chemin, renderer);
        strcpy(chemin, "skins/");
        strcat(chemin, SKINS[k]);
        strcat(chemin, "/Big.png");
        player_images[k][4] = load_image(chemin, renderer);
    }

    // images de menu - attente
    Image menu[6];
    menu[0] = load_image("loading.png", renderer);
    menu[1] = load_image("leaderboard1.png", renderer);
    menu[2] = load_image("leaderboard2.png", renderer);
    menu[3] = load_image("home.png", renderer);
    menu[4] = load_image("choix.png", renderer);
    menu[5] = load_image("regles.png", renderer);

    // chargement des sons
    //Mix_Music* music = Mix_LoadMUS("sons/son_rickroll.mp3");

    // Matrice de choix d'images
    struct plateau map = Init_map(WINDOW_HEIGHT / TILE_SIZE, WINDOW_WIDTH / TILE_SIZE);
    int** matrix = map.tabl;
    
    // initialisation des joueurs
    struct player* Joueurs[NB_j];
    int inv1[] = {10, 10, 10, 10}; 
    int inv2[] = {10, 10, 10, 10}; 
    int inv3[] = {10, 10, 10, 10}; 
    int inv4[] = {10, 10, 10, 10}; 
    struct player J1 = {true, PSEUDOS[0], 10, 1, 1, 'N', 1, inv1,4,0}; 
    struct player J2 = {true, PSEUDOS[1], 1, WINDOW_WIDTH / TILE_SIZE -2, WINDOW_HEIGHT / TILE_SIZE -2, 'N', 1, inv2,4,0};
    struct player J3 = {true, PSEUDOS[2], 1, 1, WINDOW_HEIGHT / TILE_SIZE -2, 'N', 1, inv3,4,0};
    struct player J4 = {true, PSEUDOS[3], 1, WINDOW_WIDTH / TILE_SIZE -2, 1, 'N', 1, inv4,4,0};
    Joueurs[0] = &J1;
    Joueurs[1] = &J2;
    Joueurs[2] = &J3;
    Joueurs[3] = &J4;

    int* cl = malloc(sizeof(int)*NB_j); for(int i = 0; i<NB_j; i++){cl[i] = i;}
    bool aff_cl = false;

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
    char text[10];

    // timer
    Uint32 delay = 3000; // Temps de délai en millisecondes (3 secondes)
    Uint32 wait = 1000; // temps d'exposition de l'explosion

    Uint32 bonus = - 4000;


    // Boucle principale
    SDL_Event event;
    bool fin = false;
    int quit = 0;
    int ID = 0;
    int a = 0;
    // bool dep = false;
    int home = 1;
    TTF_Init();
    Image image;
    SDL_Rect dst_rect;
    Uint32 time = 0;


    image = menu[0];
    dst_rect.x = 0;
    dst_rect.y = 0;
    dst_rect.w = image.width;
    dst_rect.h = image.height;
    SDL_RenderCopy(renderer, image.texture, NULL, &dst_rect);
    SDL_RenderPresent(renderer);

    SDL_Delay(2000);

    while (!quit) {

        while(home > 0 && !quit){
            SDL_PollEvent(&event);
            if (event.type == SDL_QUIT){
                quit = 1;
            } else if (event.type == SDL_KEYDOWN){
                switch(event.key.keysym.sym){
                    case SDLK_p : 
                        home = 0;
                        aff_cl = false;
                        fin = false;
                        a = 0;
                        map = Init_map(WINDOW_HEIGHT / TILE_SIZE, WINDOW_WIDTH / TILE_SIZE);
                        matrix = map.tabl;
                        Init_Joueurs(Joueurs);
                        image = menu[0];
                        dst_rect.x = 0;
                        dst_rect.y = 0;
                        dst_rect.w = image.width;
                        dst_rect.h = image.height;
                        SDL_RenderCopy(renderer, image.texture, NULL, &dst_rect);
                        SDL_RenderPresent(renderer);
                        SDL_Delay(2000);
                        break;
                    case SDLK_s : 
                        if(home == 1){home = 2;}
                        break;
                    case SDLK_c : 
                        if(home == 1){home = 3;}
                        break;
                    case SDLK_r : 
                        if(home > 1){home = 1;}
                        break;
                }
            }
            if (home > 0){
                image = menu[home+2];
                dst_rect.x = 0;
                dst_rect.y = 0;
                dst_rect.w = image.width;
                dst_rect.h = image.height;
                SDL_RenderCopy(renderer, image.texture, NULL, &dst_rect);
                SDL_RenderPresent(renderer);
            }
            
        }
        
        while (SDL_PollEvent(&event) && home == 0) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            } else if (event.type == SDL_KEYDOWN && Joueurs[ID]->vie > 0) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP: 
                        if (Joueurs[ID]->frame == 0){
                            move(matrix, ID, 'S', Joueurs);
                        }
                        break;
                    case SDLK_DOWN: 
                        if (Joueurs[ID]->frame == 0){
                            move(matrix, ID, 'N', Joueurs);
                        }
                        break;
                    case SDLK_LEFT:
                        if (Joueurs[ID]->frame == 0){
                            move(matrix, ID, 'O', Joueurs);
                        }
                        break;
                    case SDLK_RIGHT:
                        if (Joueurs[ID]->frame == 0){
                            move(matrix, ID, 'E', Joueurs);
                        }
                        break;

                    case SDLK_a : 
                        if (Joueurs[ID]->inventory[0] > 0 && matrix[Joueurs[ID]->y][Joueurs[ID]->x] == 0){
                            poser_item(&map, Joueurs[ID]->x, Joueurs[ID]->y, 2);
                            explose[Joueurs[ID]->y][Joueurs[ID]->x] = SDL_GetTicks()-end;
                            Joueurs[ID]->inventory[0] --;
                        }
                        break;
                    case SDLK_z : 
                        if (Joueurs[ID]->inventory[1] > 0 && matrix[Joueurs[ID]->y][Joueurs[ID]->x] == 0){
                            poser_item(&map, Joueurs[ID]->x, Joueurs[ID]->y, 3);
                            explose[Joueurs[ID]->y][Joueurs[ID]->x] = SDL_GetTicks()-end;
                            Joueurs[ID]->inventory[1] --;
                        }
                        break;
                    case SDLK_e : 
                        if (Joueurs[ID]->inventory[2] > 0 && matrix[Joueurs[ID]->y][Joueurs[ID]->x] == 0){
                            poser_item(&map, Joueurs[ID]->x, Joueurs[ID]->y, 4);
                            explose[Joueurs[ID]->y][Joueurs[ID]->x] = SDL_GetTicks()-end;
                            Joueurs[ID]->inventory[2] --;
                        }
                        break;
                    case SDLK_r : 
                        if (Joueurs[ID]->inventory[3] > 0 && matrix[Joueurs[ID]->y][Joueurs[ID]->x] == 0){
                            poser_item(&map, Joueurs[ID]->x, Joueurs[ID]->y, 5);
                            explose[Joueurs[ID]->y][Joueurs[ID]->x] = SDL_GetTicks()-end;
                            Joueurs[ID]->inventory[3] --;
                        }
                        break;
                }
            }
        }

        // gérer les explosions
        for (int i = 1; i < WINDOW_HEIGHT / TILE_SIZE -1; i++) {
            for (int j = 1; j < WINDOW_WIDTH / TILE_SIZE -1; j++) {
                if (abs(SDL_GetTicks()-end - explose[i][j] - delay) < 20 ){
                    explosion(&map, Joueurs, i, j, Bomb, boom);
                    explose[i][j] = 0;
                }
            }
        }

        // Effacement de l'écran
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        
        // Affichage du pavage d'images
        for (int i = 0; i < WINDOW_HEIGHT / TILE_SIZE; i++) {
            for (int j = 0; j < WINDOW_WIDTH / TILE_SIZE; j++) {
                image = images[matrix[i][j] + 1];
                dst_rect.x = j * TILE_SIZE;
                dst_rect.y = i * TILE_SIZE;
                dst_rect.w = image.width;
                dst_rect.h = image.height;
                SDL_RenderCopy(renderer, image.texture, NULL, &dst_rect);
                // Affichage des explosions
                if(SDL_GetTicks()-end - boom[i][j] < wait){
                    //break_bloc(&map, Joueurs, i, j);
                    image = images[13];
                    dst_rect.w = image.width;
                    dst_rect.h = image.height;
                    SDL_RenderCopy(renderer, image.texture, NULL, &dst_rect);
                }
            }
        }

        // Affichage du joueur à sa position actuelle
        for (int k = 0; k < NB_j; k++) {
            switch (Joueurs[k]->direction) {
                case 'S':
                    image = player_images[k][0];
                    break;
                case 'N':
                    image = player_images[k][1];
                    break;
                case 'E':
                    image = player_images[k][2];
                    break;
                case 'O':
                    image = player_images[k][3];
                    break;
            }
            
            dst_rect.w = image.width;
            dst_rect.h = image.height;

            if (!Joueurs[k]->etat) {
                dst_rect.x = Joueurs[k]->rank * TILE_SIZE;
                dst_rect.y = 0;
            } else {
                dst_rect.x = Joueurs[k]->x * TILE_SIZE;
                dst_rect.y = Joueurs[k]->y * TILE_SIZE;
            }

            SDL_RenderCopy(renderer, image.texture, NULL, &dst_rect);
        }

        // Affichage de la vie du joueur
        sprintf(text, "%d", Joueurs[ID]->vie);
        write(text, font, color, renderer, 0, 0);

        // Affichage du nom du joueur
        sprintf(text, "%s", Joueurs[ID]->nom);
        write(text, font, color, renderer, TILE_SIZE, WINDOW_HEIGHT - TILE_SIZE);

        // Affichage de l'inventaire du joueur
        for (int i = 0; i < 4; i++) {
            sprintf(text, "%d", Joueurs[ID]->inventory[i]);
            write(text, font, color, renderer, 0, (i+2)*TILE_SIZE);
        }
        
        // rickroll
        for(int k = 0; k < NB_j; k++){
            if (Joueurs[k]->frame == 1){
                Joueurs[k]->frame = 0;
                bonus = SDL_GetTicks();
                //Mix_PlayMusic(music, 1);
            }
        }
        if (SDL_GetTicks() - bonus < delay){
            if(SDL_GetTicks() - bonus < delay/3){
                image = images[14];
            } else if (SDL_GetTicks() - bonus > 2*delay/3){
                image = images[16];
            }else{
                image = images[15];
            }
            
            dst_rect.x = 0;
            dst_rect.y = 0;
            dst_rect.w = image.width;
            dst_rect.h = image.height;
            SDL_RenderCopy(renderer, image.texture, NULL, &dst_rect);
        }

        // fin du jeu
        if (!fin){
            a = 0;
            for (int k = 0; k<NB_j; k++){if(Joueurs[k]->etat){a++;}}
            if (a<2){fin = true; time = SDL_GetTicks(); for (int k = 0; k<NB_j; k++){Joueurs[k]->score += POINTS[Joueurs[k]->rank-1];}}
        }
        if (SDL_GetTicks() - time > delay && fin){
            if (!aff_cl){
                image = menu[1];
            } else {
                image = menu[2];          
            }
            dst_rect.x = 0;
            dst_rect.y = 0;
            dst_rect.w = image.width;
            dst_rect.h = image.height;
            SDL_RenderCopy(renderer, image.texture, NULL, &dst_rect);
            
            for (int k = 0; k < NB_j; k++){
                if (!aff_cl){
                    image = player_images[k][4];
                    dst_rect.y = 190+(Joueurs[k]->rank-1)*150;
                    sprintf(text, "%s", Joueurs[k]->nom);
                    write(text, font, color, renderer, 450, 190+(Joueurs[k]->rank-1)*150+35);
                } else {
                    image = player_images[cl[k]][4];
                    dst_rect.y = 190+k*150;
                    sprintf(text, "%s - %d", Joueurs[cl[k]]->nom, Joueurs[cl[k]]->score);
                    write(text, font, color, renderer, 450, 190+k*150+35);
                }
                dst_rect.w = image.width;
                dst_rect.h = image.height;
                dst_rect.x = 290;
                SDL_RenderCopy(renderer, image.texture, NULL, &dst_rect); 
            }
            SDL_RenderPresent(renderer);

            while (SDL_PollEvent(&event) && fin){
                if (event.type == SDL_QUIT) {
                    quit = 1;
                    fin = false;
                } else if (event.type == SDL_KEYDOWN) {
                    switch (event.key.keysym.sym){
                        case SDLK_SPACE : 
                            end = time - delay - wait;
                            aff_cl = false;
                            fin = false;
                            a = 0;
                            map = Init_map(WINDOW_HEIGHT / TILE_SIZE, WINDOW_WIDTH / TILE_SIZE);
                            matrix = map.tabl;
                            Init_Joueurs(Joueurs);
                            for (int i = 1; i < WINDOW_HEIGHT / TILE_SIZE -1; i++) {
                                for (int j = 1; j < WINDOW_WIDTH / TILE_SIZE -1; j++) {
                                    boom[i][j] = 0;
                                }
                            }
                            image = menu[0];
                            dst_rect.x = 0;
                            dst_rect.y = 0;
                            dst_rect.w = image.width;
                            dst_rect.h = image.height;
                            SDL_RenderCopy(renderer, image.texture, NULL, &dst_rect);
                            SDL_RenderPresent(renderer);
                            SDL_Delay(2000);
                            break;
                        case SDLK_c : 
                            cl = classement(Joueurs);
                            aff_cl = true;
                            break;
                        case SDLK_h : 
                            home = 1;
                            fin = false;
                            for (int i = 1; i < WINDOW_HEIGHT / TILE_SIZE -1; i++) {
                                for (int j = 1; j < WINDOW_WIDTH / TILE_SIZE -1; j++) {
                                    boom[i][j] = 0;
                                }
                            }
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
    //free(cl);
    for (int i = 0; i < 17; i++) { SDL_DestroyTexture(images[i].texture);}
    for (int i = 0; i < 6; i++) { SDL_DestroyTexture(menu[i].texture);}
    for (int i = 0; i < NB_j; i++) {for (int j = 0; j < 5; j++){SDL_DestroyTexture(player_images[i][j].texture);}}
    TTF_CloseFont(font);
    // Mix_FreeMusic(music);
    // Mix_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
