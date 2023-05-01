#ifndef __TEST_H__
#define __TEST_H__

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


bool same(int i, int j, int ID, struct player* Joueurs[NB_j]);

bool is_free(int** carte, char dir, int ID, struct player* Joueurs[NB_j]);

void move(int** carte, int ID, char dir, struct player* Joueurs[NB_j]);

void poser_item(struct plateau* plat, int x, int y, int ID_item);

int break_bloc(struct plateau*  plat, struct player* Joueurs[NB_j], int i, int j);

void explosion(struct plateau*  plat, struct player* Joueurs[NB_j], int i, int j, struct bombe Bomb[NB_b]);




#endif