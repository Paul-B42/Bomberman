# Bomberman

(manque la partie réseau)


# // Instalation //

Vous devez avoir installé SDL, SDL_image et SDL_ttf
commande Linux : sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev


# // déscriptif du jeu, les régles //

Au debut de chaque partie les 4 Joueurs commencent aux 4 coins du terrain, ils disposent tous de 10 bombes de chaque type, le but est de rester le dernier en vie. A la fin de chaque partie chaque joueurs gagne des points en fonction de son classement.

# -------------------------------------------------------------- #

# // descriptif des fonctions // 

# // 1 // le corps du jeu

 - "same" : vérifie si la case (i,j) n'est pas occupé par un joueur (joueur avec un ID différent de celui renseigné)
 - "is_free" : regarde si la case où le joueur veut aller est libre en utilisant la fonction "same"
 - "move" : déplace le joueur dans la direction souhaitée si c'est possible, sinon elle change juste son orientation (pour l'affichage), en utilisant "is_free"
 - "poser_item" : pose un item sur la carte en fonction de l'ID renseigné si la case est libre // un joueur ne peut pas reposer un item s'il en a déjà poser un à sa place
 - "break_bloc" : fait les dégâts de l'explosion d'une bombe sur une case, en cassant un bloc destructible (ID = 1) il y a une probabilité "P_bonus" de faire apparaître un bonus sur la carte (tout les bonus sont equiprobalbles). Ensuite s'il y a une bombe elle renvoie l'ID de la bombe pour la faire exploser. Puis elle met la case à 0. Enfin elle parcours la positions des joueurs et leurs retirent un point de vie si le joueur est encore en vie, si le dégâts a mit la vie du joueur à 0, elle met sont état à false (= mort) et le retire du terrain. Puis elle diminue le rang de tous les autres joueurs de 1 (utile pour le leaderboard de fin de partie)
 - "explosion" : fait les dégats (avec break_bloc) de la bombe en fonctions des caractéristiques de celle-ci, elle s'arrête à la rencontre d'un bloc indestructible et si break_bloc renvoie l'ID d'une bombe, elle l'a fait exploser; au passage elle note le temps auquel elle effectue les opérations pour l'affichage de l'explosion

 - "Init_map" : crée la catre en plaçant aléatoirement des blocs destructibles avec une probabilité "P_bloc"
 - "Init_Joueurs" : remet les données des joueurs utilisable pour une nouvelle partie
 - "classement" : classe les ID des joueurs en fonction de leurs score


# // 2 // l'affichage avec SDL

création d'un nouveau type "Image" qui contient une texture et les dimensions de l'image
 - "load_image" : crée un type "Image" à partir du chemin d'accès à l'image et du "renderer"
 - "write" : écrie le message donné en paramètre sur le renderer à l'emplacement demandé en fonction de la police et de la couleur fournies

On commence par afficher la carte comme un pavage d'images, ensuite on affiche les joueurs en fonction de leurs orientations. On écrit le nombre de points de vie et l'inventaire du joueurs

On regarde le nombre de Joueurs en vie s'il en reste un seul la partie est finie, on le laisse 3 secondes seul, puis on affiche le leaderboard, et on actualise les scores des joueurs


# // 3 // l'interface avec le joueur

Le déplacement se fait avec les flèches directionnels
Les touches "A", "Z", "E" et "R" servent à poser des bombes à l'emplacement du joueurs, rester sur une bombe fait plus de dégâts et tue sans bonus de vie
Une fois la partie terminée la touche "ESPACE" permet de relancer une nouvelle partie et la touche "C" affiche le classement général en fonction du score des Joueurs

# // 4 // La personalisation

Pour ajouter des skins il faut faire un dossier avec son nom et dans ce dossier on doit retrouver 5 images obligatoirement nommées :
    N.png ; S.png ; E.png ; O.png (le skin dans les 4 directions en 64*64) et Big.png (image pour le leaderboard en 128*128)


