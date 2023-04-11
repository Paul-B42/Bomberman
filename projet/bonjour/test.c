

struct player{
    int ID; // pour différencier les joueurs entre eux
    int vie; // entier compris entre 0 et 10
    int x; int y; // position du joueur
    char* direction; // direction du joueur nord, sud, est, ouest
    int speed; // vitesse du personnage
    int* inventory; // inventaire des objets du joueur
};


/* inventory */ // tableau de ~10 entier
// 0 - bombe classique
// 1 - bombe ++
// 2 - ...
// 3 - 
// 4 - 
// 5 - 
// 6 - 

bool is_free(int** carte, char* dir, int i, int j){
    switch (dir){
        case 'Nord' : if (carte[i+1][j] == 0){return true;};
        case 'Sud' : if (carte[i-1][j] == 0){return true;};
        case 'Est' : if (carte[i][j+1] == 0){return true;};
        case 'Ouest' : if (carte[i][j-1] == 0){return true;};
        break;
    }
}

void move(int** carte, struct player p, char* dir){
    // transformer p.x et p.y en i,j
    if (is_free(carte, dir, p.y, p.x)){
        carte[p.y][p.x] = 0;
        p.derction = dir;
        // changer i,j en x,y
        switch (dir){
            case 'Nord' : p.y += 1;
            case 'Sud' : p.y -= 1;
            case 'Est' : p.x += 1;
            case 'Ouest' : p.x -= 1;
            break;
        }
        // modifier la carte (présence joueur)
        carte[p.y][p.x] = p.ID;
    }
}

void poser_item(int** carte, struct player p, int ID_item){
    // mettre un timer pour l'explosion
    // mettre la bombe sur la map
    if (is_free(carte, p.direction, p.y, p.x)){
        // changer i,j en x,y
        switch (p.direction){
            case 'Nord' : carte[p.y+1][p.x] = p.ID_item;
            case 'Sud' : carte[p.y-1][p.x] = p.ID_item;
            case 'Est' : carte[p.y][p.x+1] = p.ID_item;
            case 'Ouest' : carte[p.y+1][p.x] = p.ID_item;
            break;
        }
        // modifier la carte (présence joueur)
    }

}

void break_bloc(int** carte, int i, int j,  struct player* p){
        if (carte[i][j] == 1){ carte[i][j] = 0;}
        else if (carte[i][j]/10 == 1 /* corespond aux ID joueurs*/){ p[carte[i][j]%10].vie -= 1} 
        else if (carte[i][j]/10 == 2 /* corespond aux ID bombes*/){ explosion(carte, carte[i][j], i, j); carte[i][j] = 0;} 
}

void explosion(struct plateau  plat,  int ID_bombe, int x, int y){ // croix de 3x3 (20)
    switch (ID_bombe){
        case 20 :
            for (int k = max(0,x-1); k <=min(plat.collone, x+1); k++ ){ // s'arrêter lors du premier bloc indestructible
                break_bloc(carte, k, y); 
            }
            for (int k = max(0,y-1); k <=min(plat.ligne, y+1); k++ ){ // s'arrêter lors du premier bloc indestructible
                break_bloc(carte, x, y);
            }
        case 21 : 
            for (int k = max(0,x-4); k <=min(plat.collone, x+4); k++ ){ // s'arrêter lors du premier bloc indestructible
                break_bloc(carte, k, y);
            }
         case 22 : 
            for (int k = max(0,y-4); k <=min(plat.ligne, y+4); k++ ){ // s'arrêter lors du premier bloc indestructible
                break_bloc(carte, x, k);
            }
        case 23 : 
            for (int k = max(0,x-2); k <=min(plat.collone, x+2); k++ ){ // s'arrêter lors du premier bloc indestructible
                break_bloc(carte, k, y); 
            }
            for (int k = max(0,y-2); k <=min(plat.ligne, y+2); k++ ){ // s'arrêter lors du premier bloc indestructible
                break_bloc(carte, x, y);
            } 
        break;
    }    
}


int main(){
    return 0;
}