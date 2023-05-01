#include "core.h"


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
    //afficher_carte(plat);
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

    

    // // définition des bombes
    // struct bombe Bomb[NB_b];
    // struct bombe B1 = {1,1}; // croix (5) // 2
    // struct bombe B2 = {2,2}; // croix (9) // 3
    // struct bombe B3 = {3,0}; // ligne (7) // 4
    // struct bombe B4 = {0,3}; // collonne (7) // 5
    // Bomb[0] = B1;
    // Bomb[1] = B2;
    // Bomb[2] = B3;
    // Bomb[3] = B4;

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
    
   

    afficher(plat, Joueurs);
    printf("\n");

    while (tic<30){
        tic++;        
        printf("Bomberman : ");
        fscanf(stdin, "%c", &touche);
        switch(touche){

            // déplacement
            case 'z' : move(plat->tabl, J, 'N', Joueurs); break;
            case 'q' : move(plat->tabl, J, 'O', Joueurs); break;
            case 's' : move(plat->tabl, J, 'S', Joueurs); break;
            case 'd' : move(plat->tabl, J, 'E', Joueurs); break;
            // bombes
            case 'b' : poser_item(plat, Joueurs[J]->x, Joueurs[J]->y, ID_bombe); break;
        }
        printf("\n");
        afficher(plat, Joueurs);
        
    }
    for (int i = 0; i < n; i++){free(carte[i]);};
    free (carte) ;
}