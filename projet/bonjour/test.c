

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

void explosion(int** carte, int ID_bombe){
    // caser les blocs, pv des blocs / joueurs -1
}


int main(){
    return 0;
}