#include<stdio.h>
#include<stdlib.h>
#include<time.h>

void affichage_carte(int ligne,int colonne,int* carte){
    printf("\n affichage de la carte \n\n");
    int k;
    int i;
    int j;
    for(j=0;j<ligne;j++){
        for (i=0;i<ligne;i++){
            k=carte[j*colonne+i];
            switch(k){
                case -1:
                    printf("M");
                    break;
                case 0:
                    printf("O");
                    break;
                case 1:
                    printf("X");
                    break;
            }
            printf(" ");
        }
        printf("\n");
    }
    printf("\n");
}

int* création_carte(int ligne, int colonne){
    srand(time(NULL));
    int n=ligne*colonne;
    int i;
    int j;
    int pos;
    int* carte = malloc(n*sizeof(int));
    for(i=0;i<n;i++){
        carte[i]=0;
    }   
    for(i=0;i<n;i++){
        pos=rand()%n; //génère un nombre aléatoire entre 0 et n-1
        carte[pos]=1; //créée un bloc déstructible à la position pos
    }
    for(i=0;i<colonne;i++){ //place des murs infranchissables en haut et en bas du terrain
        carte[i]=-1;
        carte[n-i-1]=-1;
    }
    for(i=0;i<n-colonne;i+=colonne){ //place des murs infranchissables à gauche et à droite du terrain
        carte[i]=-1;
        carte[n-i-1]=-1;
    }
    for(j=2;j<ligne-1;j+=2){ //place des murs infranchissables dans l'aire de jeux
        for (i=2;i<colonne-1;i+=2){
            carte[j*colonne+i]=-1;
        }
    }
    carte[ligne+1]=0; //vide les positions de départs des joueurs
    carte[ligne+2]=0;
    carte[2*ligne+1]=0;
    carte[2*ligne-2]=0;
    carte[2*ligne-3]=0;
    carte[3*ligne-2]=0;
    carte[n-2*ligne+1]=0;
    carte[n-2*ligne+2]=0;
    carte[n-3*ligne+1]=0;
    carte[n-ligne-2]=0;
    carte[n-ligne-3]=0;
    carte[n-2*ligne-2]=0;
    return carte;
}

int main(){
    int ligne=13;
    int colonne=13;
    int* carte=création_carte(ligne,colonne);
    affichage_carte(ligne,colonne,carte);
    free(carte);
    return 0;
}