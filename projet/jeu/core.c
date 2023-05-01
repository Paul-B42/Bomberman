#include "core.h"


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