#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#define min(i, j) (((i) < (j)) ? (i) : (j))
#define max(i, j) (((i) > (j)) ? (i) : (j))

int NB_j = 4; // nombre de joueurs
int NB_b = 4; // nombre de bombes

struct player{
    char* nom; // nom du joueur
    int vie; // entier compris entre 0 et 10
    int x; int y; // position du joueur
    char direction; // direction du joueur nord, sud, est, ouest
    int speed; // vitesse du personnage
    int* inventory; // inventaire des objets du joueur
};

struct plateau{
    int** tabl;
    int lignes;
    int collonnes;
};

struct bombe{
    int L;
    int C;
};

struct explose{
    int i;
    int j; 
    int t; //time_t t;
};

/* inventory */ // tableau de ~10 entier
// 0 - bombe classique
// 1 - bombe ++
// 2 - ...
// 3 - 
// 4 - 
// 5 - 
// 6 - 


// fonction testée et validée
bool same(int i, int j, int ID, struct player* Joueurs[NB_j]){
    for (int k = 0; k < NB_j; k++){
        if (k != ID){if(Joueurs[k]->y == i && Joueurs[k]->x == j){
            return true;
        }}
    }
    return false;
}

// fonction validée et testée
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


// fonction validée et testée
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


// fonction validée et testée
void poser_item(struct plateau* plat, int x, int y, int ID_item){
    // mettre un timer pour l'explosion
    plat->tabl[y][x] = ID_item;
}


// fonction validée et testée
int break_bloc(struct plateau*  plat, struct player* Joueurs[NB_j], int i, int j){ // ajouter l'animation de l'explosion
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


// fonction validée et testée
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


// fonction validée et testée
void afficher_carte(struct plateau* plat){
    for (int i = plat->lignes-1; i >= 0 ; i--){
        for (int j = 0; j < plat->collonnes; j++){
            switch (plat->tabl[i][j]){
                case -1 :printf(" ✖ "); break;
                case 0 : printf("   "); break;
                case 1 : printf(" ◻ "); break;
                default :printf(" B "); break;
            }
        } printf("\n");
    }
}

void afficher(struct plateau* plat, struct player* Joueurs[NB_j]){
    for (int i = plat->lignes-1; i >= 0 ; i--){
        for (int j = 0; j < plat->collonnes; j++){
            if (same(i,j, -1, Joueurs)){
                printf(" ○ ");
            }else{
            switch (plat->tabl[i][j]){
                case -1 : printf(" ✖ "); break;
                case 0 : printf("   "); break;
                case 1 : printf(" ◻ "); break;
                default : printf(" B "); break;
            }}
        } printf("\n");
    }
}

int main(int argc, char *argv[]){
    
    // int n = 7; // taille du tableau
    // int m = 7;

    // struct player* Joueurs[NB_j];
    // struct player J1 = {"Romain", 10, 1, 1, 'N', 1, NULL};
    // struct player J2 = {"Christophe", 10, n-2, m-2, 'S', 1, NULL};
    // struct player J3 = {"Orso", 10, 1, m-2, 'E', 1, NULL};
    // struct player J4 = {"Paul", 10, n-2, 1, 'O', 1, NULL};
    // Joueurs[0] = &J1;
    // Joueurs[1] = &J2;
    // Joueurs[2] = &J3;
    // Joueurs[3] = &J4;

    // struct bombe Bomb[NB_b];
    // struct bombe B1 = {1,1}; // croix (5) // 2
    // struct bombe B2 = {2,2}; // croix (9) // 3
    // struct bombe B3 = {3,0}; // ligne (7) // 4
    // struct bombe B4 = {0,3}; // collonne (7) // 5
    // Bomb[0] = B1;
    // Bomb[1] = B2;
    // Bomb[2] = B3;
    // Bomb[3] = B4;

    // int **carte = malloc (n * sizeof (int*)) ; // création de la carte
    // for (int i = 0; i < n; i++)
    //     carte[i] = malloc (m * sizeof (int)) ;

    // for(int k =0; k<n; k++){ // attention bordures pour une carte carré
    //     carte[0][k] = -1;
    //     carte[n-1][k] = -1;
    //     carte[k][0] = -1;
    //     carte[k][n-1] = -1;
    // }
    

    // for (int i = 1; i<n-1; i++){
    //     for (int j = 1; j < m-1; j++){
    //         carte[i][j] = 1;
    //     }
    // }

    // struct player Bonjour = {"Corentin", 10, 3, 3, 'N', 1, NULL};
    
    // struct plateau plat = {carte, n, m};

    // // ok // test de fonction indique si c'est la case où se trouve un joueur
    // if(same(5,5,2,Joueurs)){
    //     printf("Il y a un joueur sur cette case \n");
    // }else{
    //     printf("Il n'y a pas de joueurs sur cette case \n");
    // }
    // printf("Hello World ! \n\n");

    // // ok // test de la fonction est_libre
    // if(is_free(carte, 'N', 1, Joueurs)){
    //     printf("Oui la case est libre\n");
    // }else{
    //     printf("Non la case est occupée\n");
    // }
    // printf("Hello World ! \n\n");

    // // ok // test de la fonction de déplacement
    // printf("%s se trouve en %d ; %d\n", Bonjour.nom, Joueurs[1]->x, Joueurs[1]->y);
    // move(carte, 1, 'S', Joueurs); 
    // printf("%s se trouve en %d ; %d\n", Bonjour.nom, Joueurs[1]->x, Joueurs[1]->y);
    // move(carte, 1, 'O', Joueurs);
    // printf("%s se trouve en %d ; %d\n", Bonjour.nom, Joueurs[1]->x, Joueurs[1]->y);
    // printf("Hello World ! \n\n");
    
    // // ok // test de la fonction pour poser des items 
    // poser_item(plat, Bonjour.x, Bonjour.y, 4);
    // printf("Une bombe a été poser en %d ; %d , c'est une bombe : %d \n", Bonjour.x, Bonjour.y, carte[Bonjour.y][Bonjour.x]);
    // printf("Hello World ! \n\n");

    // // ok // test du tableau de struct
    // for(int i = 0; i<NB_j; i++){printf("Joueur %d : %s \n", i+1, Joueurs[i]->nom);};
    // printf("Hello World ! \n\n");

    // // ok // test de la fonction pour casser des blocs et faire des dégats 
    // break_bloc(&plat, Joueurs, 2, 1);
    // break_bloc(&plat, Joueurs, 1, 1);
    // printf("%s n'a plus que %d points de vie : %d\n", Joueurs[0]->nom, Joueurs[0]->vie, carte[2][1]);
    // printf("Hello World ! \n\n");

    // // ok // test de la fonction explosion
    // carte[1][1] = 1;
    // carte[2][1] = 1;
    // carte[4][2] = 3;
    // carte[2][2] = 4;
    // carte[4][3] = -1;
    // afficher_carte(plat);
    // explosion(plat, Joueurs, 4, 2, Bomb);
    // printf("\n");
    // afficher_carte(plat);
    // printf("Hello World ! \n\n");

    // for (int i = 0; i < n; i++){free(carte[i]);};
    // free (carte) ;

    printf("// - - - - - - - - - - - - - - - - - - - - - //\n");
    printf("// - - - Fin des tests et place au jeu - - - //\n");
    printf("// - - - - - - - - - - - - - - - - - - - - - //\n");

    // - - - Fin des tests et place au jeu - - - //

    // initialisation de la carte 

    int n = 7; int m = 9;
    int **carte = malloc (n * sizeof (int*)) ; // création de la carte
    for (int i = 0; i < n; i++){ carte[i] = malloc (m * sizeof (int)) ;}
    struct plateau plat1 = {carte, n, m};
    struct plateau* plat = &plat1;

    for(int i = 0; i<n; i++){ // bordures de la carte
        carte[i][0] = -1;
        carte[i][m-1] = -1;
    }
    // afficher_carte(plat);
    printf("Hello World ! \n\n");
    for(int j = 0; j<m; j++){
        carte[0][j] = -1;
        carte[n-1][j] = -1;
    }
    // afficher_carte(plat);
    printf("Hello World ! \n\n");
    for (int i = 1; i<n-1; i++){ // murs cassables
        for (int j = 1; j < m-1; j++){
            carte[i][j] = 1;
        }
    }
    afficher_carte(plat);
    printf("Hello World ! \n\n");
    for(int i2 = 2; i2+2<n; i2+=2){ // ajout des murs incassables
        for(int j2 = 2; j2+2<m; j2+=2){
            carte[i2][j2] = -1;
        }
    }
    // afficher_carte(plat);
    printf("Hello World ! \n\n");
    carte[1][1] = 0; carte[2][1] = 0; carte[1][2] = 0; // blocs pour le départ des joueurs
    carte[1][m-2] = 0; carte[2][m-2] = 0; carte[1][m-3] = 0;
    carte[n-2][1] = 0; carte[n-3][1] = 0; carte[n-2][2] = 0;
    carte[n-2][m-2] = 0; carte[n-3][m-2] = 0; carte[n-2][m-3] = 0;
    // afficher_carte(plat);
    printf("Hello World ! \n\n");

    

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

    // initialisation des joueurs

    struct player* Joueurs[NB_j];
    struct player J1 = {"Romain", 10, 1, 1, 'N', 1, NULL}; // noms avec des nscanf
    struct player J2 = {"Christophe", 10, m-2, n-2, 'S', 1, NULL};
    struct player J3 = {"Orso", 10, 1, n-2, 'E', 1, NULL};
    struct player J4 = {"Paul", 10, m-2, 1, 'O', 1, NULL};
    Joueurs[0] = &J1;
    Joueurs[1] = &J2;
    Joueurs[2] = &J3;
    Joueurs[3] = &J4;

    // jeu

    poser_item(plat, 3, 3, 4);

    char touche;
    int J = 1;
    int ID_bombe = 3;
    int tic = 0;
    int texp = 3;
    
    struct explose* boom[texp];
    for(int k = 0; k<texp; k++){
        struct explose b = {0,0,0};
        boom[k] = &b;
    }

    afficher(plat, Joueurs);
    printf("\n");

    while (tic<30){
        tic++; printf("%d ; %d // %d ; %d // %d ; %d\n", tic, tic%3, Joueurs[J]->x, Joueurs[J]->y, boom[tic%texp]->i, boom[tic%texp]->j);
        // if(boom[tic%texp]->t + texp == tic){/*explosion(plat, Joueurs, boom[tic%texp]->i, boom[tic%texp]->j, Bomb)*/printf("boom %d ; %d \n", boom[tic%texp]->i, boom[tic%texp]->j);}
        printf("Bomberman : ");
        fscanf(stdin, "%c", &touche);
        switch(touche){
        
            // déplacement
            case 'z' : move(plat->tabl, J, 'N', Joueurs); break;
            case 'q' : move(plat->tabl, J, 'O', Joueurs); break;
            case 's' : move(plat->tabl, J, 'S', Joueurs); break;
            case 'd' : move(plat->tabl, J, 'E', Joueurs); break;
            // bombes
            case 'b' : poser_item(plat, Joueurs[J]->x, Joueurs[J]->y, ID_bombe); 
            boom[tic%texp]->i = Joueurs[J]->y;
            boom[tic%texp]->j = Joueurs[J]->x;
            boom[tic%texp]->t = tic;
            break;
        }
        printf("\n");
        afficher(plat, Joueurs);
        
    }


    // time_t debut = time(NULL);
    // char touche;
    // int J = 1;
    // struct explose boom;
    // int k = 0; int ID_bombe = 3;
    // while (difftime(time(NULL), debut) < 300) { // parties de 5 minutes
    //     printf("Bomberman : ");
    //     fscanf(stdin, "%c", &touche);
    //     switch(touche){
        
    //         // déplacement
    //         case 'z' : move(carte, J, 'N', Joueurs); break;
    //         case 'q' : move(carte, J, 'O', Joueurs); break;
    //         case 's' : move(carte, J, 'S', Joueurs); break;
    //         case 'd' : move(carte, J, 'E', Joueurs); break;

    //         // poser une bombe
    //         // case 'b' : poser_item(carte, Joueurs[J]->x, Joueurs[J]->y, ID_bombe+2); explosion(&plat, Joueurs, Joueurs[J]->y, Joueurs[J]->x, Bomb);
    //             //boom.t = time(NULL); boom.i = Joueurs[J]->y; boom.j = Joueurs[J]->x; break;
    //     }  
    //     // if (difftime(time(NULL), boom.t) >= 3 && difftime(time(NULL), boom.t) < 4){ 
    //     //     explosion(&plat, Joueurs, boom.i, boom.j, Bomb);
    //     //     boom.t = debut;
    //     // } 
    //     // if(difftime(time(NULL), debut) > 10*k){
    //     //     afficher(plat, Joueurs);
    //     //     printf("\n\n");
    //     //     k++;
    //     // }
    //     afficher(plat, Joueurs);
    // }

    for (int i = 0; i < n; i++){free(carte[i]);};
    free (carte) ;
}