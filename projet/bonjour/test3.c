#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

// Fonction de callback appelée lorsque le délai du timer est atteint
Uint32 timerCallback(Uint32 interval, void *param) {
    // On récupère le message à afficher
    char *message = (char*) param;
    printf("%s\n", message);
    return 0; // Valeur de retour 0 pour arrêter le timer
}

int main() {
    // Initialisation de SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf("Erreur d'initialisation de SDL : %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Création de la fenêtre
    SDL_Window *window = SDL_CreateWindow("Exemple de timer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Erreur de création de la fenêtre : %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Création du renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Erreur de création du renderer : %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    // Variables de la boucle principale
    SDL_Event event;
    bool quit = false;

    while (!quit) {
        // Gestion des événements
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_KEYDOWN) {
                char message[64];
                switch (event.key.keysym.sym) {
                    case SDLK_UP: sprintf(message, "UP"); break;
                    case SDLK_DOWN: sprintf(message, "DOWN"); break;
                    case SDLK_LEFT: sprintf(message, "LEFT"); break;
                    case SDLK_RIGHT: sprintf(message, "RIGHT"); break;
                    case SDLK_a : sprintf(message, "A"); break;
                    case SDLK_b : sprintf(message, "B"); break;
                    case SDLK_c : sprintf(message, "C"); break;
                    case SDLK_d : sprintf(message, "D"); break;
                    case SDLK_e : sprintf(message, "E"); break;
                    case SDLK_f : sprintf(message, "F"); break;
                    case SDLK_g : sprintf(message, "G"); break;
                    case SDLK_h : sprintf(message, "H"); break;
                    case SDLK_i : sprintf(message, "I"); break;
                    case SDLK_j : sprintf(message, "J"); break;
                    case SDLK_k : sprintf(message, "K"); break;
                    case SDLK_l : sprintf(message, "L"); break;
                    case SDLK_m : sprintf(message, "M"); break;
                    case SDLK_n : sprintf(message, "N"); break;
                    case SDLK_o : sprintf(message, "O"); break;
                    case SDLK_p : sprintf(message, "P"); break;
                    case SDLK_q : sprintf(message, "Q"); break;
                    case SDLK_r : sprintf(message, "R"); break;
                    case SDLK_s : sprintf(message, "S"); break;
                    case SDLK_t : sprintf(message, "T"); break;
                    case SDLK_u : sprintf(message, "U"); break;
                    case SDLK_v : sprintf(message, "V"); break;
                    case SDLK_w : sprintf(message, "W"); break;
                    case SDLK_x : sprintf(message, "X"); break;
                    case SDLK_y : sprintf(message, "Y"); break;
                    case SDLK_z : sprintf(message, "Z"); break;
                    default: sprintf(message, "Touche inconnue"); break;
                    }

                printf("Appuyé sur la touche %s\n", message);

            // Ajout d'un délai de 3 secondes avec un timer
            Uint32 delay = 3000; // 3 secondes
            SDL_TimerID timerID = SDL_AddTimer(delay, timerCallback, (void *)message);
            if (timerID == 0) {
                printf("Erreur lors de la création du timer : %s\n", SDL_GetError());
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                SDL_Quit();
                return EXIT_FAILURE;
            }
        }
    }

    // Autres opérations de la boucle principale...

    SDL_Delay(10); // Délai court pour éviter une consommation excessive du processeur
}

SDL_DestroyRenderer(renderer);
SDL_DestroyWindow(window);
SDL_Quit();

return EXIT_SUCCESS;

}