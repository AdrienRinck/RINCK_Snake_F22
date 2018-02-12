#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <windows.h>

#define BORDURE '#'
#define SNAKEHEAD 'H'
#define SNAKEBODY 'B'
#define TAILLEMAXSNAKE 3304
#define CAKE 'O'

enum BOOL{True,False};

typedef struct Coordonnee
{
    int x;
    int y;
} Coordonnee;

typedef struct Configuration
{
    int Largeur;
    int Hauteur;
    int difficulty;
} Configuration;

typedef struct Cake
{
    int posX, posY;
} Cake;

typedef struct Snake
{
    Coordonnee position; // Donne la position de la tete
    Coordonnee direction; // Donne sa direction
    int taille; // Sa taille
    int score; // Le score du joueur
    Coordonnee corpsCoord[TAILLEMAXSNAKE];
} Snake;

//PROTOTYPE
void gotoxy(int x, int y);
void AfficherCarte(const Configuration conf);
void DemanderConfiguration(Configuration *conf);
void AfficherSnake(const Snake *s);
void GenererSnake(Snake *s, const Configuration *conf);
int generatedOnSnake(const Snake *s, const Cake *c);
int randomBetweenNumber(int min, int max);
void GenerateCake(const Snake *s, const Configuration *conf, Cake *c);
void AfficherCake (const Cake *c);
void HandleInput(Snake *s);
void EraseSnake(const Snake *s);
int NotCollideWithBody(const Snake *s);
int isPlaying(const Snake *s, const Configuration *conf);
void Move(Snake *s);
int onCake(const Snake *s, const Cake *c);
void growSnake(Snake *s);
void PlaySnake();


int main()
{
    srand(time(0));
    PlaySnake();
    return 0;
}
//----------GOTOXY----------//
void gotoxy(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

//----------Affichage de la carte en fonction des input----------//
void AfficherCarte(const Configuration conf)
{
    int i, j;
    gotoxy(0, 0);
    for(i = 0; i < conf.Largeur; i++)
    {
        printf("%c", BORDURE);
    }
    printf("\n");

    for(i = 0; i < conf.Hauteur - 2; i++)
    {
        printf("%c", BORDURE);
        for (j = 0; j < conf.Largeur - 2; j++)
        {
            printf(" ");
        }
        printf("%c\n", BORDURE);
    }

    for(i = 0; i < conf.Largeur; i++)
    {
        printf("%c", BORDURE);
    }
}
//----------Set de la taille de la carte----------//
void DemanderConfiguration(Configuration *conf)
{
    do
    {
        printf("Largeur de la carte (entre 30 et 120): ");
        scanf("%d", &(*conf).Largeur);
    }while((*conf).Largeur < 30 || (*conf).Largeur > 120);

    do
    {
        printf("Hauteur de la carte (entre 15 et 30): ");
        scanf("%d", &(*conf).Hauteur);
    }while((*conf).Hauteur < 15 || (*conf).Hauteur > 30);

    do
    {
        printf("Difficulte (1 = easy, 2 = medium, 3 = hard): ");
        scanf("%d", &(*conf).difficulty);
    }while((*conf).difficulty < 1 || (*conf).difficulty > 3);
}
//----------Affichage du snake en fonction de sa taille----------//
void AfficherSnake(const Snake *s)
{
    gotoxy(s->position.x, s->position.y);
    printf("%c", SNAKEHEAD);

    int i;
    for(i=1; i < s->taille; ++i)
    {
        gotoxy(s->corpsCoord[i].x, s->corpsCoord[i].y);
        printf("%c", SNAKEBODY);
    }
}

//----------Affichage dynamique du snake----------//
void GenererSnake(Snake *s, const Configuration *conf)
{
    s->direction.x = 1; // Il pars vers la droite
    s->direction.y = 0;

    s->position.x = conf->Largeur/2; // On place le snake au milieu du terrain
    s->position.y = conf->Hauteur/2;

    s->corpsCoord[0] = s->position; // Positionne la tête au bon endroit
    s->corpsCoord[1].x = conf->Largeur/2 - 1; // Le corps se positionne en fonction de la tete
    s->corpsCoord[1].y = conf->Hauteur/2;
    s->taille = 2; // Ca taille vaut donc 2

    s->score = 0; // Set de score
}

int generatedOnSnake(const Snake *s, const Cake *c)
{
    int i = 0;
    while(i < s->taille && (s->corpsCoord[i].x != c->posX && s->corpsCoord[i].y != c->posY))
        ++i;

    return i != s->taille;
}
//----------Random----------//
int randomBetweenNumber(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

//----------Generation des cakes----------//
void GenerateCake(const Snake *s, const Configuration *conf, Cake *c)
{
    do
    {
        c->posX = randomBetweenNumber(1, conf->Largeur - 2);
        c->posY = randomBetweenNumber(1, conf->Hauteur - 2);
    }while(generatedOnSnake(s, c));
}

//----------Affichage des cakes----------//
void AfficherCake (const Cake *c)
{
    gotoxy(c->posX, c->posY);
    printf("%C", CAKE);
}

//----------Gestion de l'input----------//
void HandleInput(Snake *s) // Procedure OnKeyPress
{
    if(GetAsyncKeyState(VK_UP) && s->direction.y != 1)  //HAUT
    {
        s->direction.x = 0;
        s->direction.y = -1;
    }
    else if(GetAsyncKeyState(VK_DOWN) && s->direction.y != -1)  //BAS
    {
        s->direction.x = 0;
        s->direction.y = 1;
        return;
    }
    else if(GetAsyncKeyState(VK_RIGHT) && s->direction.x != - 1)  //DROITE
    {
        s->direction.x = 1;
        s->direction.y = 0;
    }
    else if(GetAsyncKeyState(VK_LEFT) && s->direction.x != 1)   //GAUCHE
    {
        s->direction.x = -1;
        s->direction.y = 0;
    }
}

//----------Procedure d'effacement du snake----------//
void EraseSnake(const Snake *s)
{
    int i;
    for(i = 0; i < s->taille; i++)
    {
        gotoxy(s->corpsCoord[i].x, s->corpsCoord[i].y);
        printf(" ");
    }
}

//----------Collision avec le corps du snake----------//
int NotCollideWithBody(const Snake *s)
{
    int i = 1;
    while((i < s->taille) && ((s->corpsCoord[0].x != s->corpsCoord[i].x) || (s->corpsCoord[0].y != s->corpsCoord[i].y)))
    {
        ++i;
    }
    return i == s->taille;
}

//----------Verification des collisions eliminatoire----------//
int isPlaying(const Snake *s, const Configuration *conf)
{
    return (s->position.x < conf->Largeur - 1 && s->position.x >= 1 && s->position.y >= 1 && s->position.y < conf->Hauteur - 1 && NotCollideWithBody(s));
}

//----------Deplacement du snake----------//
void Move(Snake *s)
{
    int i;
    for(i = s->taille - 1; i >= 1; --i)
    {
        s->corpsCoord[i] = s->corpsCoord[i - 1];
    }

    // Bouge la tête
    s->corpsCoord[0].x += s->direction.x;
    s->corpsCoord[0].y += s->direction.y;

    // Change sa position
    s->position.x += s->direction.x;
    s->position.y += s->direction.y;
}

//----------Collision avec le cake----------//
int onCake(const Snake *s, const Cake *c)
{
   return ((s->position.x == c->posX) && (s->position.y == c->posY));
}

//----------Aggrandi la taille du snake apres eat----------//
void growSnake(Snake *s)
{
    s->corpsCoord[s->taille].x = s->corpsCoord[s->taille - 1].x - s->direction.x;
    s->corpsCoord[s->taille].y = s->corpsCoord[s->taille - 1].y - s->direction.y;
    s->taille++;
}
//----------Boucle de jeux principale----------//
void PlaySnake()
{
    Configuration conf;
    Snake s;
    Cake c;

    DemanderConfiguration(&conf);
    GenererSnake(&s, &conf);
    GenerateCake(&s, &conf ,&c);
    system("CLS");

    // Affiche les composants du jeu
    AfficherCarte(conf);
    AfficherSnake(&s);
    AfficherCake(&c);

    while(isPlaying(&s, &conf))
    {
        HandleInput(&s);
        EraseSnake(&s);
        Move(&s);
        if(onCake(&s, &c))
        {
            s.score += 100;
            growSnake(&s);
            GenerateCake(&s, &conf, &c);
            AfficherCake(&c);
        }
        AfficherSnake(&s);
        Sleep(100 - (conf.difficulty - 1) * 20);
    }
    system("cls");
    printf("Bien jouer! Vous avez fait un score de: %d\nAppuyer sur une touche pour quitter.", s.score);
    system("pause > NUL");
}

