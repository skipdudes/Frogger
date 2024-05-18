#define _CRT_SECURE_NO_WARNINGS

#include <SDL.h>
#include <stdio.h>
#include <string.h>


//***************************************************
//*******     STALE, ZMIENNE I STRUKTURY     ********
//***************************************************

// szerokosc i wysokosc ekranu
#define SCREEN_WIDTH	672
#define SCREEN_HEIGHT	768

// ilosc obiektow
#define CAR1_NUMBER 6
#define CAR2_NUMBER 6
#define CAR3_NUMBER 6
#define CAR4_NUMBER 2
#define CAR5_NUMBER 4
#define TURTLE1_NUMBER 4
#define TURTLE1DIVE_NUMBER 2
#define LOG1_NUMBER 6
#define LOG2_NUMBER 4
#define TURTLE2_NUMBER 5
#define TURTLE2DIVE_NUMBER 3
#define LOG3_NUMBER 6

// z iloma zyciami zaczyna grac
#define LIVES_NUMBER 3

// potrzebne zmienne
int t1, t2, selectedMenu, frames, score, lives, moving, dying, happySaved, timeRect, timeCopy, gameFrame, overAnim, typedPos;
double delta, worldTime, passTime, fpsTimer, fps;
bool typed, happy[5], fullScreen, movingUp[12], active, over, done, quit, pause, wantsToQuit, deathType; //0 - wybucha, 1 - tonie
char text[28];

// zmienne potrzebne do zapisywania high-scores
FILE* highscores;
int scores[10];
char names[10][12];
int highestScore;
char name[128];

// kolory
int red;
int green;
int black;

// zmienne globalne
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Surface* screen = NULL;
SDL_Surface* charset = NULL;
SDL_Texture* scrtex = NULL;
SDL_Event event;

// struktury
struct frog
{
	enum direction
	{
		UP = 'u', DOWN = 'd', LEFT = 'l', RIGHT = 'r'
	} dir;

	struct
	{
		double x;
		int y;
	} pos;

	SDL_Surface* sprite;
} hero;

struct car
{
	enum direction
	{
		LEFT = 'l', RIGHT = 'r'
	} dir;

	struct
	{
		double x;
		int y;
	} pos;
	bool att; //0 - friend, 1 - enemy
	double speed;
	SDL_Surface* sprite;
} car1[CAR1_NUMBER], car2[CAR2_NUMBER], car3[CAR3_NUMBER], car4[CAR4_NUMBER], car5[CAR5_NUMBER], turtle1[TURTLE1_NUMBER], turtle1dive[TURTLE1DIVE_NUMBER], logg1[LOG1_NUMBER], logg2[LOG2_NUMBER], turtle2[TURTLE2_NUMBER], turtle2dive[TURTLE2DIVE_NUMBER], logg3[LOG3_NUMBER];


//***************************************************
//*****************     BITMAPY     *****************
//***************************************************
SDL_Surface* bfrog1a1 = NULL, * bfrog1a2 = NULL, * bfrog1d1 = NULL, * bfrog1d2 = NULL, * bfrog1s1 = NULL, * bfrog1s2 = NULL, * bfrog1w1 = NULL, * bfrog1w2 = NULL;
SDL_Surface* bfrog2w1 = NULL;
SDL_Surface* bturtle1_1 = NULL, * bturtle1_2 = NULL, * bturtle1_3 = NULL, * bturtle1_4 = NULL, * bturtle1_5 = NULL, * bturtle1_6 = NULL;
SDL_Surface* bturtle2_1 = NULL, * bturtle2_2 = NULL, * bturtle2_3 = NULL, * bturtle2_4 = NULL, * bturtle2_5 = NULL, * bturtle2_6 = NULL;
SDL_Surface* bcar1_1 = NULL; SDL_Surface* bcar1_2 = NULL;
SDL_Surface* bcar2_1 = NULL; SDL_Surface* bcar2_2 = NULL;
SDL_Surface* bcar3_1 = NULL; SDL_Surface* bcar3_2 = NULL;
SDL_Surface* bcar4_1 = NULL; SDL_Surface* bcar4_2 = NULL;
SDL_Surface* bcar5_1 = NULL; SDL_Surface* bcar5_2 = NULL;
SDL_Surface* bhappyfrog = NULL;
SDL_Surface* blevel = NULL;
SDL_Surface* blife = NULL;
SDL_Surface* blog1 = NULL;
SDL_Surface* blog2 = NULL;
SDL_Surface* blog3 = NULL;
SDL_Surface* bmap = NULL;
SDL_Surface* bdie1_1 = NULL; SDL_Surface* bdie1_2 = NULL; SDL_Surface* bdie1_3 = NULL;
SDL_Surface* bdie2_1 = NULL; SDL_Surface* bdie2_2 = NULL; SDL_Surface* bdie2_3 = NULL;
SDL_Surface* bdiemark = NULL;
SDL_Surface* bback = NULL;
SDL_Surface* blogotitle = NULL;


//***************************************************
//*****************     FUNKCJE     *****************
//***************************************************

// zaladuj szare napisy do SDL_Surface* charset
bool loadGrayCharset()
{
	SDL_FreeSurface(charset);
	bool success = true;

	charset = SDL_LoadBMP("Sprites/Charset/gray.bmp");
	if (charset == NULL)
	{

		success = false;
	}
	else SDL_SetColorKey(charset, true, 0x000000);

	return success;
}


// zaladuj czerwone napisy do SDL_Surface* charset
bool loadRedCharset()
{
	SDL_FreeSurface(charset);
	bool success = true;

	charset = SDL_LoadBMP("Sprites/Charset/red.bmp");
	if (charset == NULL)
	{

		success = false;
	}
	else SDL_SetColorKey(charset, true, 0x000000);

	return success;
}


// zaladuj turkusowe napisy do SDL_Surface* charset
bool loadTurquoiseCharset()
{
	SDL_FreeSurface(charset);
	bool success = true;

	charset = SDL_LoadBMP("Sprites/Charset/turquoise.bmp");
	if (charset == NULL)
	{

		success = false;
	}
	else SDL_SetColorKey(charset, true, 0x000000);

	return success;
}


// zaladuj fioletowe napisy do SDL_Surface* charset
bool loadVioletCharset()
{
	SDL_FreeSurface(charset);
	bool success = true;

	charset = SDL_LoadBMP("Sprites/Charset/violet.bmp");
	if (charset == NULL)
	{

		success = false;
	}
	else SDL_SetColorKey(charset, true, 0x000000);

	return success;
}


// zaladuj zolte napisy do SDL_Surface* charset
bool loadYellowCharset()
{
	SDL_FreeSurface(charset);
	bool success = true;

	charset = SDL_LoadBMP("Sprites/Charset/yellow.bmp");
	if (charset == NULL)
	{

		success = false;
	}
	else SDL_SetColorKey(charset, true, 0x000000);

	return success;
}


// narysowanie napisu txt na powierzchni screen, zaczynajπc od punktu (x, y)
void DrawString(SDL_Surface* screen, int x, int y, const char* text, SDL_Surface* charset)
{
	x++;
	y++;

	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text)
	{
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		d.w = 24;
		d.h = 24;
		SDL_BlitScaled(charset, &s, screen, &d);
		x += 24;
		text++;
	};
};


// narysowanie na ekranie screen powierzchni sprite w punkcie (x, y)
void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y)
{
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	dest.w = (sprite->w) * 3;
	dest.h = (sprite->h) * 3;
	SDL_BlitScaled(sprite, NULL, screen, &dest);
};


// rysowanie pojedynczego pixela
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color)
{
	int bpp = surface->format->BytesPerPixel;
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32*)p = color;
};


// rysowanie linii o d≥ugoúci l w pionie (gdy dx = 0, dy = 1) bπdü poziomie (gdy dx = 1, dy = 0)
void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color)
{
	for (int i = 0; i < l; i++)
	{
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	};
};


// rysowanie prostokπta o d≥ugoúci bokÛw l i k
void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k, Uint32 color)
{
	for (int i = x; i > x - l; i--)
		DrawLine(screen, i, y + 1, k, 0, 1, color);
};


// inicjuje SDL, tworzy okno, renderer i jego wlasciwosci
bool init()
{
	bool success = true;

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{

		success = false;
	}

	int rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);

	if (rc != 0)
	{
		SDL_Quit();

		success = false;
	};

	if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear") != SDL_TRUE)
	{

		success = false;
	}

	if (SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT) != 0)
	{

		success = false;
	}

	if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255) != 0)
	{

		success = false;
	}

	SDL_SetWindowTitle(window, "Frogger");

	SDL_Surface* icon = SDL_LoadBMP("Sprites/icon.bmp");
	SDL_SetColorKey(icon, true, 0x000000);
	SDL_SetWindowIcon(window, icon);
	SDL_FreeSurface(icon);

	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

	SDL_ShowCursor(SDL_DISABLE);

	red = SDL_MapRGB(screen->format, 0xE0, 0x00, 0x00);
	green = SDL_MapRGB(screen->format, 0x1D, 0xC3, 0x00);
	black = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);

	return success;
}


// przelacza miedzy trybem okna a pelnym ekranem
void setFullScreen()
{
	fullScreen = !fullScreen;
	if (!fullScreen)
		SDL_SetWindowFullscreen(window, 0);
	else if (fullScreen)
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
}


// wczytuje obrazy do zmiennych SDL_Surface
bool load()
{
	bool success = true;

	bfrog1a1 = SDL_LoadBMP("Sprites/Frogger/frog1a1.bmp"); if (bfrog1a1 == NULL) { success = false; }
	else SDL_SetColorKey(bfrog1a1, true, 0x000000);
	bfrog1a2 = SDL_LoadBMP("Sprites/Frogger/frog1a2.bmp"); if (bfrog1a2 == NULL) { success = false; }
	else SDL_SetColorKey(bfrog1a2, true, 0x000000);
	bfrog1d1 = SDL_LoadBMP("Sprites/Frogger/frog1d1.bmp"); if (bfrog1d1 == NULL) { success = false; }
	else SDL_SetColorKey(bfrog1d1, true, 0x000000);
	bfrog1d2 = SDL_LoadBMP("Sprites/Frogger/frog1d2.bmp"); if (bfrog1d2 == NULL) { success = false; }
	else SDL_SetColorKey(bfrog1d2, true, 0x000000);
	bfrog1s1 = SDL_LoadBMP("Sprites/Frogger/frog1s1.bmp"); if (bfrog1s1 == NULL) { success = false; }
	else SDL_SetColorKey(bfrog1s1, true, 0x000000);
	bfrog1s2 = SDL_LoadBMP("Sprites/Frogger/frog1s2.bmp"); if (bfrog1s2 == NULL) { success = false; }
	else SDL_SetColorKey(bfrog1s2, true, 0x000000);
	bfrog1w1 = SDL_LoadBMP("Sprites/Frogger/frog1w1.bmp"); if (bfrog1w1 == NULL) { success = false; }
	else SDL_SetColorKey(bfrog1w1, true, 0x000000);
	bfrog1w2 = SDL_LoadBMP("Sprites/Frogger/frog1w2.bmp"); if (bfrog1w2 == NULL) { success = false; }
	else SDL_SetColorKey(bfrog1w2, true, 0x000000);
	bfrog2w1 = SDL_LoadBMP("Sprites/Frogger/frog2w1.bmp"); if (bfrog2w1 == NULL) { success = false; }
	else SDL_SetColorKey(bfrog2w1, true, 0x000000);

	bturtle1_1 = SDL_LoadBMP("Sprites/Turtle/turtle1_1.bmp"); if (bturtle1_1 == NULL) { success = false; }
	else SDL_SetColorKey(bturtle1_1, true, 0x000000);
	bturtle1_2 = SDL_LoadBMP("Sprites/Turtle/turtle1_2.bmp"); if (bturtle1_2 == NULL) { success = false; }
	else SDL_SetColorKey(bturtle1_2, true, 0x000000);
	bturtle1_3 = SDL_LoadBMP("Sprites/Turtle/turtle1_3.bmp"); if (bturtle1_3 == NULL) { success = false; }
	else SDL_SetColorKey(bturtle1_3, true, 0x000000);
	bturtle1_4 = SDL_LoadBMP("Sprites/Turtle/turtle1_4.bmp"); if (bturtle1_4 == NULL) { success = false; }
	else SDL_SetColorKey(bturtle1_4, true, 0x000000);
	bturtle1_5 = SDL_LoadBMP("Sprites/Turtle/turtle1_5.bmp"); if (bturtle1_5 == NULL) { success = false; }
	else SDL_SetColorKey(bturtle1_5, true, 0x000000);
	bturtle1_6 = SDL_LoadBMP("Sprites/Turtle/turtle1_6.bmp"); if (bturtle1_6 == NULL) { success = false; }
	else SDL_SetColorKey(bturtle1_6, true, 0x000000);
	bturtle2_1 = SDL_LoadBMP("Sprites/Turtle/turtle2_1.bmp"); if (bturtle2_1 == NULL) { success = false; }
	else SDL_SetColorKey(bturtle2_1, true, 0x000000);
	bturtle2_2 = SDL_LoadBMP("Sprites/Turtle/turtle2_2.bmp"); if (bturtle2_2 == NULL) { success = false; }
	else SDL_SetColorKey(bturtle2_2, true, 0x000000);
	bturtle2_3 = SDL_LoadBMP("Sprites/Turtle/turtle2_3.bmp"); if (bturtle2_3 == NULL) { success = false; }
	else SDL_SetColorKey(bturtle2_3, true, 0x000000);
	bturtle2_4 = SDL_LoadBMP("Sprites/Turtle/turtle2_4.bmp"); if (bturtle2_4 == NULL) { success = false; }
	else SDL_SetColorKey(bturtle2_4, true, 0x000000);
	bturtle2_5 = SDL_LoadBMP("Sprites/Turtle/turtle2_5.bmp"); if (bturtle2_5 == NULL) { success = false; }
	else SDL_SetColorKey(bturtle2_5, true, 0x000000);
	bturtle2_6 = SDL_LoadBMP("Sprites/Turtle/turtle2_6.bmp"); if (bturtle2_6 == NULL) { success = false; }
	else SDL_SetColorKey(bturtle2_6, true, 0x000000);

	bcar1_1 = SDL_LoadBMP("Sprites/Car/car1_1.bmp"); if (bcar1_1 == NULL) { success = false; }
	else SDL_SetColorKey(bcar1_1, true, 0x000000);
	bcar1_2 = SDL_LoadBMP("Sprites/Car/car1_2.bmp"); if (bcar1_2 == NULL) { success = false; }
	else SDL_SetColorKey(bcar1_2, true, 0x000000);
	bcar2_1 = SDL_LoadBMP("Sprites/Car/car2_1.bmp"); if (bcar2_1 == NULL) { success = false; }
	else SDL_SetColorKey(bcar2_1, true, 0x000000);
	bcar2_2 = SDL_LoadBMP("Sprites/Car/car2_2.bmp"); if (bcar2_2 == NULL) { success = false; }
	else SDL_SetColorKey(bcar2_2, true, 0x000000);
	bcar3_1 = SDL_LoadBMP("Sprites/Car/car3_1.bmp"); if (bcar3_1 == NULL) { success = false; }
	else SDL_SetColorKey(bcar3_1, true, 0x000000);
	bcar3_2 = SDL_LoadBMP("Sprites/Car/car3_2.bmp"); if (bcar3_2 == NULL) { success = false; }
	else SDL_SetColorKey(bcar3_2, true, 0x000000);
	bcar4_1 = SDL_LoadBMP("Sprites/Car/car4_1.bmp"); if (bcar4_1 == NULL) { success = false; }
	else SDL_SetColorKey(bcar4_1, true, 0x000000);
	bcar4_2 = SDL_LoadBMP("Sprites/Car/car4_2.bmp"); if (bcar4_2 == NULL) { success = false; }
	else SDL_SetColorKey(bcar4_2, true, 0x000000);
	bcar5_1 = SDL_LoadBMP("Sprites/Car/car5_1.bmp"); if (bcar5_1 == NULL) { success = false; }
	else SDL_SetColorKey(bcar5_1, true, 0x000000);
	bcar5_2 = SDL_LoadBMP("Sprites/Car/car5_2.bmp"); if (bcar5_2 == NULL) { success = false; }
	else SDL_SetColorKey(bcar5_2, true, 0x000000);

	bhappyfrog = SDL_LoadBMP("Sprites/happyfrog.bmp"); if (bhappyfrog == NULL) { success = false; }
	else SDL_SetColorKey(bhappyfrog, true, 0x000000);
	blevel = SDL_LoadBMP("Sprites/level.bmp"); if (blevel == NULL) { success = false; }
	else SDL_SetColorKey(blevel, true, 0x000000);
	blife = SDL_LoadBMP("Sprites/life.bmp"); if (blife == NULL) { success = false; }
	else SDL_SetColorKey(blife, true, 0x000000);

	blog1 = SDL_LoadBMP("Sprites/log1.bmp"); if (blog1 == NULL) { success = false; }
	else SDL_SetColorKey(blog1, true, 0x000000);
	blog2 = SDL_LoadBMP("Sprites/log2.bmp"); if (blog2 == NULL) { success = false; }
	else SDL_SetColorKey(blog2, true, 0x000000);
	blog3 = SDL_LoadBMP("Sprites/log3.bmp"); if (blog3 == NULL) { success = false; }
	else SDL_SetColorKey(blog3, true, 0x000000);

	bmap = SDL_LoadBMP("Sprites/map.bmp"); if (bmap == NULL) { success = false; }

	bdie1_1 = SDL_LoadBMP("Sprites/Die/die1_1.bmp"); if (bdie1_1 == NULL) { success = false; }
	else SDL_SetColorKey(bdie1_1, true, 0x000000);
	bdie1_2 = SDL_LoadBMP("Sprites/Die/die1_2.bmp"); if (bdie1_2 == NULL) { success = false; }
	else SDL_SetColorKey(bdie1_2, true, 0x000000);
	bdie1_3 = SDL_LoadBMP("Sprites/Die/die1_3.bmp"); if (bdie1_3 == NULL) { success = false; }
	else SDL_SetColorKey(bdie1_3, true, 0x000000);
	bdie2_1 = SDL_LoadBMP("Sprites/Die/die2_1.bmp"); if (bdie2_1 == NULL) { success = false; }
	else SDL_SetColorKey(bdie2_1, true, 0x000000);
	bdie2_2 = SDL_LoadBMP("Sprites/Die/die2_2.bmp"); if (bdie2_2 == NULL) { success = false; }
	else SDL_SetColorKey(bdie2_2, true, 0x000000);
	bdie2_3 = SDL_LoadBMP("Sprites/Die/die2_3.bmp"); if (bdie2_3 == NULL) { success = false; }
	else SDL_SetColorKey(bdie2_3, true, 0x000000);
	bdiemark = SDL_LoadBMP("Sprites/Die/diemark.bmp"); if (bdiemark == NULL) { success = false; }
	else SDL_SetColorKey(bdiemark, true, 0x000000);

	bback = SDL_LoadBMP("Sprites/back.bmp"); if (bback == NULL) { success = false; }
	blogotitle = SDL_LoadBMP("Sprites/logotitle.bmp"); if (blogotitle == NULL) { success = false; }
	else SDL_SetColorKey(blogotitle, true, 0x000000);

	return success;
}


// rysuje zycia
void drawLives()
{
	for (int i = 0; i < lives * 24; i += 24)
	{
		DrawSurface(screen, blife, i, 720);
	}
}


// rysuje czas
void drawTime()
{
	//360px - 50s
	//7.2px - 1s
	double mnoznik = 7.2 * (50 - worldTime);
	int rzutnik = int(mnoznik);

	if (50 - worldTime <= 10)
	{
		DrawRectangle(screen, 576, 744, rzutnik, 24, red);
		loadGrayCharset();
	}
	else
	{
		DrawRectangle(screen, 576, 744, rzutnik, 24, green);
		loadYellowCharset();
	}
	sprintf(text, "TIME"); DrawString(screen, 576, 744, text, charset);
}


// zwalnia pamiec
void free()
{
	SDL_FreeSurface(bfrog1a1); SDL_FreeSurface(bfrog1a2); SDL_FreeSurface(bfrog1d1); SDL_FreeSurface(bfrog1d2); SDL_FreeSurface(bfrog1s1); SDL_FreeSurface(bfrog1s2); SDL_FreeSurface(bfrog1w1); SDL_FreeSurface(bfrog1w2);
	SDL_FreeSurface(bfrog2w1);
	SDL_FreeSurface(bturtle1_1); SDL_FreeSurface(bturtle1_2); SDL_FreeSurface(bturtle1_3); SDL_FreeSurface(bturtle1_4); SDL_FreeSurface(bturtle1_5); SDL_FreeSurface(bturtle1_6);
	SDL_FreeSurface(bturtle2_1); SDL_FreeSurface(bturtle2_2); SDL_FreeSurface(bturtle2_3); SDL_FreeSurface(bturtle2_4); SDL_FreeSurface(bturtle2_5); SDL_FreeSurface(bturtle2_6);
	SDL_FreeSurface(bcar1_1); SDL_FreeSurface(bcar1_2);
	SDL_FreeSurface(bcar2_1); SDL_FreeSurface(bcar2_2);
	SDL_FreeSurface(bcar3_1); SDL_FreeSurface(bcar3_2);
	SDL_FreeSurface(bcar4_1); SDL_FreeSurface(bcar4_2);
	SDL_FreeSurface(bcar5_1); SDL_FreeSurface(bcar5_2);
	SDL_FreeSurface(bhappyfrog);
	SDL_FreeSurface(blevel);
	SDL_FreeSurface(blife);
	SDL_FreeSurface(blog1);
	SDL_FreeSurface(blog2);
	SDL_FreeSurface(blog3);
	SDL_FreeSurface(bmap);
	SDL_FreeSurface(bdie1_1); SDL_FreeSurface(bdie1_2); SDL_FreeSurface(bdie1_3);
	SDL_FreeSurface(bdie2_1); SDL_FreeSurface(bdie2_2); SDL_FreeSurface(bdie2_3);
	SDL_FreeSurface(bdiemark);
	SDL_FreeSurface(bback);
	SDL_FreeSurface(blogotitle);

	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
}


// resetuje zmienne sprawdzajace postep ruchu gracza
void resetMovingUp()
{
	for (int i = 0; i < 12; i++)
		movingUp[i] = 0;
}


// umieszcza gracza w poczatkowej pozycji
void moveHeroAtStart()
{
	moving = 0;
	hero.dir = frog::direction::UP;
	hero.pos.x = 336;
	hero.pos.y = 672;
	hero.sprite = bfrog1w1;
	resetMovingUp();
}


// wypisz "enter name: "
void drawEnterName()
{
	DrawRectangle(screen, 636, 390, 600, 36, black);

	if (!loadRedCharset()) { printf("loadRedCharset error\n"); return; }
	sprintf(text, "ENTER NAME: "); DrawString(screen, 60, 396, text, charset);

	if (!loadYellowCharset()) { printf("loadYellowCharset error\n"); return; }
	sprintf(text, "%s", name); DrawString(screen, 348, 396, text, charset);
}


// zamienia napis1 na napis2
void zamien(char napis1[], char napis2[])
{
	for (int i = 0; i < strlen(napis1); i++)
	{
		napis1[i] = '\0';
	}
	for (int i = 0; i < strlen(napis2); i++)
	{
		napis1[i] = napis2[i];
	}
	napis1[strlen(napis2)] = '\0';
}


// laduje najlepsze wyniki
void loadHighScores()
{
	highscores = fopen("highscores.bin", "r");
	if (highscores)
	{

	}
	if (highscores == NULL)
	{
		//creating file
		highscores = fopen("highscores.bin", "w");
		if (highscores)
		{

		}
		fprintf(highscores, "4000 SANS\n2600 ASGORE\n1460 UNDYNE\n800 TOBYFOX\n780 TORIEL\n720 PAPYRUS\n600 GASTER\n120 FLOWEY\n100 FRISK\n10 CHARA\n");
		fclose(highscores);
		loadHighScores();
	}
	else
	{
		for (int i = 0; i < 10; i++)
		{
			fscanf(highscores, "%d %11s\n", &scores[i], &names[i]);
			if (scores[i] > highestScore)
			{
				highestScore = scores[i];
			}
		}
		fclose(highscores);
	}
}


// wypisuje 10 najlepszych wynikow
void drawHighScores()
{
	loadTurquoiseCharset();
	sprintf(text, "HI-SCORES"); DrawString(screen, 228, 96, text, charset);

	sprintf(text, "#1 %05d %s", scores[0], names[0]); if (typedPos == 0) loadVioletCharset(); DrawString(screen, 96, 168, text, charset); if (typedPos == 0) loadTurquoiseCharset();
	sprintf(text, "#2 %05d %s", scores[1], names[1]); if (typedPos == 1) loadVioletCharset(); DrawString(screen, 96, 216, text, charset); if (typedPos == 1) loadTurquoiseCharset();
	sprintf(text, "#3 %05d %s", scores[2], names[2]); if (typedPos == 2) loadVioletCharset(); DrawString(screen, 96, 264, text, charset); if (typedPos == 2) loadTurquoiseCharset();
	sprintf(text, "#4 %05d %s", scores[3], names[3]); if (typedPos == 3) loadVioletCharset(); DrawString(screen, 96, 312, text, charset); if (typedPos == 3) loadTurquoiseCharset();
	sprintf(text, "#5 %05d %s", scores[4], names[4]); if (typedPos == 4) loadVioletCharset(); DrawString(screen, 96, 360, text, charset); if (typedPos == 4) loadTurquoiseCharset();
	sprintf(text, "#6 %05d %s", scores[5], names[5]); if (typedPos == 5) loadVioletCharset(); DrawString(screen, 96, 408, text, charset); if (typedPos == 5) loadTurquoiseCharset();
	sprintf(text, "#7 %05d %s", scores[6], names[6]); if (typedPos == 6) loadVioletCharset(); DrawString(screen, 96, 456, text, charset); if (typedPos == 6) loadTurquoiseCharset();
	sprintf(text, "#8 %05d %s", scores[7], names[7]); if (typedPos == 7) loadVioletCharset(); DrawString(screen, 96, 504, text, charset); if (typedPos == 7) loadTurquoiseCharset();
	sprintf(text, "#9 %05d %s", scores[8], names[8]); if (typedPos == 8) loadVioletCharset(); DrawString(screen, 96, 552, text, charset); if (typedPos == 8) loadTurquoiseCharset();
	sprintf(text, "#10 %05d %s", scores[9], names[9]); if (typedPos == 9) loadVioletCharset(); DrawString(screen, 72, 600, text, charset); if (typedPos == 9) loadTurquoiseCharset();

	sprintf(text, "PRESS -ESC- OR -ENTER-"); DrawString(screen, 72, 696, text, charset);
}


// zapisuje hi-scores do pliku
void saveHighscores()
{
	highscores = fopen("highscores.bin", "w");
	if (highscores == NULL)
	{

	}
	else
	{
		for (int i = 0; i < 10; i++)
		{
			fprintf(highscores, "%d %s\n", scores[i], names[i]);
		}
		fclose(highscores);
	}
}


// aktualizuje tabele
void moveDownTable(int n)
{
	for (int i = 9; i > n; i--)
	{
		scores[i] = scores[i - 1];
		zamien(names[i], names[i - 1]);
	}
}


// nadaje zmiennym i obiektom poczatkowe statystyki (ogolnym)
void start()
{
	loadHighScores();
	selectedMenu = 0;
	fullScreen = 0;
	quit = 0;
	gameFrame = 0;
}


// nadaje zmiennym i obiektom poczatkowe statystyki (potrzebnym w grze)
void startTime()
{
	t1 = SDL_GetTicks();
	frames = 0;
	score = 0;
	lives = LIVES_NUMBER;
	fpsTimer = 0;
	fps = 0;
	worldTime = 0;
	passTime = 0;
	active = 1;
	moving = 0;
	dying = 0;
	over = 0;
	deathType = 0;
	happySaved = 0;
	done = 0;
	pause = 0;
	wantsToQuit = 0;
	timeRect = 0;
	timeCopy = 0;
	overAnim = 0;
	typed = 0;
	typedPos = 10;

	for (int i = 0; i < 5; i++)
		happy[i] = 0;

	resetMovingUp();
	moveHeroAtStart();

	//car1
	for (int i = 0; i < CAR1_NUMBER; i++)
	{
		car1[i].dir = car::direction::LEFT;
		car1[i].pos.y = 624;
		car1[i].att = 1;
		car1[i].speed = 0.8;//3
		car1[i].sprite = bcar1_1;
	}
	car1[0].pos.x = 240;
	car1[1].pos.x = 456;
	car1[2].pos.x = 672;
	car1[3].pos.x = 1056;
	car1[4].pos.x = 1248;
	car1[5].pos.x = 1440;

	//car2
	for (int i = 0; i < CAR2_NUMBER; i++)
	{
		car2[i].dir = car::direction::RIGHT;
		car2[i].pos.y = 576;
		car2[i].att = 1;
		car2[i].speed = 0.8;//3
		car2[i].sprite = bcar2_1;
	}
	car2[0].pos.x = 288;
	car2[1].pos.x = 480;
	car2[2].pos.x = 672;
	car2[3].pos.x = 1056;
	car2[4].pos.x = 1248;
	car2[5].pos.x = 1440;

	//car3
	for (int i = 0; i < CAR3_NUMBER; i++)
	{
		car3[i].dir = car::direction::LEFT;
		car3[i].pos.y = 528;
		car3[i].att = 1;
		car3[i].speed = 1.7;//4
		car3[i].sprite = bcar3_1;
	}
	car3[0].pos.x = 288;
	car3[1].pos.x = 480;
	car3[2].pos.x = 672;
	car3[3].pos.x = 1008;
	car3[4].pos.x = 1224;
	car3[5].pos.x = 1440;

	//car4
	for (int i = 0; i < CAR4_NUMBER; i++)
	{
		car4[i].dir = car::direction::RIGHT;
		car4[i].pos.y = 480;
		car4[i].att = 1;
		car4[i].speed = 2;//5
		car4[i].sprite = bcar4_1;
	}
	car4[0].pos.x = -96;
	car4[1].pos.x = 672;

	//car5
	for (int i = 0; i < CAR5_NUMBER; i++)
	{
		car5[i].dir = car::direction::LEFT;
		car5[i].pos.y = 432;
		car5[i].att = 1;
		car5[i].speed = 1.5;//6
		car5[i].sprite = bcar5_1;
	}
	car5[0].pos.x = 360;
	car5[1].pos.x = 624;
	car5[2].pos.x = 1080;
	car5[3].pos.x = 1344;

	//turtle1
	for (int i = 0; i < TURTLE1_NUMBER; i++)
	{
		turtle1[i].dir = car::direction::LEFT;
		turtle1[i].pos.y = 336;
		turtle1[i].att = 0;
		turtle1[i].speed = 1.1;//4
		turtle1[i].sprite = bturtle1_1;
	}
	turtle1[0].pos.x = 0;
	turtle1[1].pos.x = 192;
	turtle1[2].pos.x = 576;
	turtle1[3].pos.x = 768;

	//turtle1dive
	for (int i = 0; i < TURTLE1DIVE_NUMBER; i++)
	{
		turtle1dive[i].dir = car::direction::LEFT;
		turtle1dive[i].pos.y = 336;
		turtle1dive[i].att = 0;
		turtle1dive[i].speed = 1.1;//4
		turtle1dive[i].sprite = bturtle1_1;
	}
	turtle1dive[0].pos.x = -192;
	turtle1dive[1].pos.x = 384;

	//logg1
	for (int i = 0; i < LOG1_NUMBER; i++)
	{
		logg1[i].dir = car::direction::RIGHT;
		logg1[i].pos.y = 288;
		logg1[i].att = 0;
		logg1[i].speed = 1;//3
		logg1[i].sprite = blog1;
	}
	logg1[0].pos.x = -192;
	logg1[1].pos.x = 96;
	logg1[2].pos.x = 336;
	logg1[3].pos.x = 576;
	logg1[4].pos.x = 864;
	logg1[5].pos.x = 1104;

	//logg2
	for (int i = 0; i < LOG2_NUMBER; i++)
	{
		logg2[i].dir = car::direction::RIGHT;
		logg2[i].pos.y = 240;
		logg2[i].att = 0;
		logg2[i].speed = 1.8;//6
		logg2[i].sprite = blog2;
	}
	logg2[0].pos.x = -336;
	logg2[1].pos.x = 48;
	logg2[2].pos.x = 432;
	logg2[3].pos.x = 816;

	//turtle2
	for (int i = 0; i < TURTLE2_NUMBER; i++)
	{
		turtle2[i].dir = car::direction::LEFT;
		turtle2[i].pos.y = 192;
		turtle2[i].att = 0;
		turtle2[i].speed = 1.3;//4
		turtle2[i].sprite = bturtle2_1;
	}
	turtle2[0].pos.x = 120;
	turtle2[1].pos.x = 288;
	turtle2[2].pos.x = 624;
	turtle2[3].pos.x = 888;
	turtle2[4].pos.x = 1224;

	//turtle2dive
	for (int i = 0; i < TURTLE2DIVE_NUMBER; i++)
	{
		turtle2dive[i].dir = car::direction::LEFT;
		turtle2dive[i].pos.y = 192;
		turtle2dive[i].att = 0;
		turtle2dive[i].speed = 1.3;//4
		turtle2dive[i].sprite = bturtle2_1;
	}
	turtle2dive[0].pos.x = -144;
	turtle2dive[1].pos.x = 456;
	turtle2dive[2].pos.x = 1056;

	//logg3
	for (int i = 0; i < LOG3_NUMBER; i++)
	{
		logg3[i].dir = car::direction::RIGHT;
		logg3[i].pos.y = 144;
		logg3[i].att = 0;
		logg3[i].speed = 1.2;//4
		logg3[i].sprite = blog3;
	}
	logg3[0].pos.x = -240;
	logg3[1].pos.x = 0;
	logg3[2].pos.x = 264;
	logg3[3].pos.x = 528;
	logg3[4].pos.x = 768;
	logg3[5].pos.x = 1032;
}


// rozpoczyna zdarzenie, gdzie gracz MOZE stracic zycie
void die(bool x)
{
	if (dying == 0 && active == 1)
	{
		deathType = x;
		active = 0;
		moving = 0;
		hero.dir = frog::direction::UP;
		dying = 140;
	}
}


// zarzadza zyciami, zmiennymi i graczem po smierci
void afterDeath()
{
	lives--;
	if (lives > 0)
	{
		moveHeroAtStart();
		worldTime = 0;
		active = 1;
	}
	if (lives == 0)
	{
		over = 1;
		overAnim = 192;
	}
}


// animuje smierc gracza
void dieToDeath()
{
	if (dying > 0)
	{
		if (dying >= 110)
		{
			if (deathType == 0) hero.sprite = bdie1_1;
			else if (deathType == 1) hero.sprite = bdie2_1;
		}
		else if (dying < 110 && dying >= 80)
		{
			if (deathType == 0) hero.sprite = bdie1_2;
			else if (deathType == 1) hero.sprite = bdie2_2;
		}
		else if (dying < 80 && dying >= 50)
		{
			if (deathType == 0) hero.sprite = bdie1_3;
			else if (deathType == 1) hero.sprite = bdie2_3;
		}
		else if (dying < 50 && dying > 0)
		{
			hero.sprite = bdiemark;
		}
		dying--;
	}
}


// sprawdza na jakim etapie smierci jest gracz i czy w ogole umiera
void checkDying()
{
	if (dying > 0 && lives > 0)
	{
		dieToDeath();
	}
	else if (dying == 0 && active == 0 && lives > 0)
	{
		if (over == 0) afterDeath();
	}
}


// funkcje odpowiedzialne za ruch poszczegolnych grup obiektow
void moveCar1()
{
	for (int i = 0; i < CAR1_NUMBER; i++)
	{
		if (car1[i].dir == car::direction::LEFT)
		{
			car1[i].pos.x -= car1[i].speed;
			if (car1[i].pos.x <= -48)
			{
				car1[i].pos.x = 1440;
			}
		}

		if (car1[i].dir == car::direction::RIGHT)
		{
			car1[i].pos.x += car1[i].speed;
			if (car1[i].pos.x >= 1440)
			{
				car1[i].pos.x = -48;
			}
		}
	}
}
void moveCar2()
{
	for (int i = 0; i < CAR2_NUMBER; i++)
	{
		if (car2[i].dir == car::direction::LEFT)
		{
			car2[i].pos.x -= car2[i].speed;
			if (car2[i].pos.x <= -48)
			{
				car2[i].pos.x = 1440;
			}
		}

		if (car2[i].dir == car::direction::RIGHT)
		{
			car2[i].pos.x += car2[i].speed;
			if (car2[i].pos.x >= 1440)
			{
				car2[i].pos.x = -48;
			}
		}
	}
}
void moveCar3()
{
	for (int i = 0; i < CAR3_NUMBER; i++)
	{
		if (car3[i].dir == car::direction::LEFT)
		{
			car3[i].pos.x -= car3[i].speed;
			if (car3[i].pos.x <= -48)
			{
				car3[i].pos.x = 1440;
			}
		}

		if (car3[i].dir == car::direction::RIGHT)
		{
			car3[i].pos.x += car3[i].speed;
			if (car3[i].pos.x >= 1440)
			{
				car3[i].pos.x = -48;
			}
		}
	}
}
void moveCar4()
{
	for (int i = 0; i < CAR4_NUMBER; i++)
	{
		if (car4[i].dir == car::direction::LEFT)
		{
			car4[i].pos.x -= car4[i].speed;
			if (car4[i].pos.x <= -96)
			{
				car4[i].pos.x = 1440;
			}
		}

		if (car4[i].dir == car::direction::RIGHT)
		{
			car4[i].pos.x += car4[i].speed;
			if (car4[i].pos.x >= 1440)
			{
				car4[i].pos.x = -96;
			}
		}
	}
}
void moveCar5()
{
	for (int i = 0; i < CAR5_NUMBER; i++)
	{
		if (car5[i].dir == car::direction::LEFT)
		{
			car5[i].pos.x -= car5[i].speed;
			if (car5[i].pos.x <= -96)
			{
				car5[i].pos.x = 1392;
			}
		}

		if (car5[i].dir == car::direction::RIGHT)
		{
			car5[i].pos.x += car5[i].speed;
			if (car5[i].pos.x >= 1392)
			{
				car5[i].pos.x = -96;
			}
		}
	}
}
void moveTurtle1()
{
	for (int i = 0; i < TURTLE1_NUMBER; i++)
	{
		if (turtle1[i].dir == car::direction::LEFT)
		{
			turtle1[i].pos.x -= turtle1[i].speed;
			if (turtle1[i].pos.x <= -192)
			{
				turtle1[i].pos.x = 960;
			}
		}

		if (turtle1[i].dir == car::direction::RIGHT)
		{
			turtle1[i].pos.x += turtle1[i].speed;
			if (turtle1[i].pos.x >= 960)
			{
				turtle1[i].pos.x = -192;
			}
		}
	}
}
void moveTurtle1dive()
{
	for (int i = 0; i < TURTLE1DIVE_NUMBER; i++)
	{
		if (turtle1dive[i].dir == car::direction::LEFT)
		{
			turtle1dive[i].pos.x -= turtle1dive[i].speed;
			if (turtle1dive[i].pos.x <= -192)
			{
				turtle1dive[i].pos.x = 960;
			}
		}

		if (turtle1dive[i].dir == car::direction::RIGHT)
		{
			turtle1dive[i].pos.x += turtle1dive[i].speed;
			if (turtle1dive[i].pos.x >= 960)
			{
				turtle1dive[i].pos.x = -192;
			}
		}
	}
}
void moveLog1()
{
	for (int i = 0; i < LOG1_NUMBER; i++)
	{
		if (logg1[i].dir == car::direction::LEFT)
		{
			logg1[i].pos.x -= logg1[i].speed;
			if (logg1[i].pos.x <= -192)
			{
				logg1[i].pos.x = 1344;
			}
		}

		if (logg1[i].dir == car::direction::RIGHT)
		{
			logg1[i].pos.x += logg1[i].speed;
			if (logg1[i].pos.x >= 1344)
			{
				logg1[i].pos.x = -192;
			}
		}
	}
}
void moveLog2()
{
	for (int i = 0; i < LOG2_NUMBER; i++)
	{
		if (logg2[i].dir == car::direction::LEFT)
		{
			logg2[i].pos.x -= logg2[i].speed;
			if (logg2[i].pos.x <= -336)
			{
				logg2[i].pos.x = 1200;
			}
		}

		if (logg2[i].dir == car::direction::RIGHT)
		{
			logg2[i].pos.x += logg2[i].speed;
			if (logg2[i].pos.x >= 1200)
			{
				logg2[i].pos.x = -336;
			}
		}
	}
}
void moveTurtle2()
{
	for (int i = 0; i < TURTLE2_NUMBER; i++)
	{
		if (turtle2[i].dir == car::direction::LEFT)
		{
			turtle2[i].pos.x -= turtle2[i].speed;
			if (turtle2[i].pos.x <= -144)
			{
				turtle2[i].pos.x = 1392;
			}
		}

		if (turtle2[i].dir == car::direction::RIGHT)
		{
			turtle2[i].pos.x += turtle2[i].speed;
			if (turtle2[i].pos.x >= 1392)
			{
				turtle2[i].pos.x = -144;
			}
		}
	}
}
void moveTurtle2dive()
{
	for (int i = 0; i < TURTLE2DIVE_NUMBER; i++)
	{
		if (turtle2dive[i].dir == car::direction::LEFT)
		{
			turtle2dive[i].pos.x -= turtle2dive[i].speed;
			if (turtle2dive[i].pos.x <= -144)
			{
				turtle2dive[i].pos.x = 1392;
			}
		}

		if (turtle2dive[i].dir == car::direction::RIGHT)
		{
			turtle2dive[i].pos.x += turtle2dive[i].speed;
			if (turtle2dive[i].pos.x >= 1392)
			{
				turtle2dive[i].pos.x = -144;
			}
		}
	}
}
void moveLog3()
{
	for (int i = 0; i < LOG3_NUMBER; i++)
	{
		if (logg3[i].dir == car::direction::LEFT)
		{
			logg3[i].pos.x -= logg3[i].speed;
			if (logg3[i].pos.x <= -240)
			{
				logg3[i].pos.x = 1296;
			}
		}

		if (logg3[i].dir == car::direction::RIGHT)
		{
			logg3[i].pos.x += logg3[i].speed;
			if (logg3[i].pos.x >= 1296)
			{
				logg3[i].pos.x = -240;
			}
		}
	}
}


// zmienia polozenie poruszajacych sie obiektow
void moveMovingObjects()
{
	moveCar1();
	moveCar2();
	moveCar3();
	moveCar4();
	moveCar5();
	moveTurtle1();
	moveTurtle1dive();
	moveLog1();
	moveLog2();
	moveTurtle2();
	moveTurtle2dive();
	moveLog3();
}


// funkcje odpowiedzialne za rysowanie poszczegolnych grup obiektow
void drawCar1()
{
	for (int i = 0; i < CAR1_NUMBER; i++)
		DrawSurface(screen, car1[0].sprite, int(car1[i].pos.x), car1[i].pos.y);
}
void drawCar2()
{
	for (int i = 0; i < CAR2_NUMBER; i++)
		DrawSurface(screen, car2[0].sprite, int(car2[i].pos.x), car2[i].pos.y);
}
void drawCar3()
{
	for (int i = 0; i < CAR3_NUMBER; i++)
		DrawSurface(screen, car3[0].sprite, int(car3[i].pos.x), car3[i].pos.y);
}
void drawCar4()
{
	for (int i = 0; i < CAR4_NUMBER; i++)
		DrawSurface(screen, car4[0].sprite, int(car4[i].pos.x), car4[i].pos.y);
}
void drawCar5()
{
	for (int i = 0; i < CAR5_NUMBER; i++)
		DrawSurface(screen, car5[0].sprite, int(car5[i].pos.x), car5[i].pos.y);
}
void drawTurtle1()
{
	for (int i = 0; i < TURTLE1_NUMBER; i++)
		DrawSurface(screen, turtle1[0].sprite, int(turtle1[i].pos.x), turtle1[i].pos.y);
}
void drawTurtle1dive()
{
	for (int i = 0; i < TURTLE1DIVE_NUMBER; i++)
		DrawSurface(screen, turtle1dive[0].sprite, int(turtle1dive[i].pos.x), turtle1dive[i].pos.y);
}
void drawLog1()
{
	for (int i = 0; i < LOG1_NUMBER; i++)
		DrawSurface(screen, logg1[0].sprite, int(logg1[i].pos.x), logg1[i].pos.y);
}
void drawLog2()
{
	for (int i = 0; i < LOG2_NUMBER; i++)
		DrawSurface(screen, logg2[0].sprite, int(logg2[i].pos.x), logg2[i].pos.y);
}
void drawTurtle2()
{
	for (int i = 0; i < TURTLE2_NUMBER; i++)
		DrawSurface(screen, turtle2[0].sprite, int(turtle2[i].pos.x), turtle2[i].pos.y);
}
void drawTurtle2dive()
{
	for (int i = 0; i < TURTLE2DIVE_NUMBER; i++)
		DrawSurface(screen, turtle2dive[0].sprite, int(turtle2dive[i].pos.x), turtle2dive[i].pos.y);
}
void drawLog3()
{
	for (int i = 0; i < LOG3_NUMBER; i++)
		DrawSurface(screen, logg3[0].sprite, int(logg3[i].pos.x), logg3[i].pos.y);
}


// rysuje poruszajace sie obiekty
void drawMovingObjects()
{
	drawCar1();
	drawCar2();
	drawCar3();
	drawCar4();
	drawCar5();
	drawTurtle1();
	drawTurtle1dive();
	drawLog1();
	drawLog2();
	drawTurtle2();
	drawTurtle2dive();
	drawLog3();
}


// zarzadza animacjami samochodow
void animateCars()
{
	if ((passTime - int(passTime) >= 0.0 && passTime - int(passTime) < 0.25) || (passTime - int(passTime) >= 0.5 && passTime - int(passTime) < 0.75))
	{
		for (int i = 0; i < CAR1_NUMBER; i++)
			car1[i].sprite = bcar1_1;
		for (int i = 0; i < CAR2_NUMBER; i++)
			car2[i].sprite = bcar2_1;
		for (int i = 0; i < CAR3_NUMBER; i++)
			car3[i].sprite = bcar3_1;
		for (int i = 0; i < CAR4_NUMBER; i++)
			car4[i].sprite = bcar4_1;
		for (int i = 0; i < CAR5_NUMBER; i++)
			car5[i].sprite = bcar5_1;
	}
	else
	{
		for (int i = 0; i < CAR1_NUMBER; i++)
			car1[i].sprite = bcar1_2;
		for (int i = 0; i < CAR2_NUMBER; i++)
			car2[i].sprite = bcar2_2;
		for (int i = 0; i < CAR3_NUMBER; i++)
			car3[i].sprite = bcar3_2;
		for (int i = 0; i < CAR4_NUMBER; i++)
			car4[i].sprite = bcar4_2;
		for (int i = 0; i < CAR5_NUMBER; i++)
			car5[i].sprite = bcar5_2;
	}
}


// zarzadza animacjami zolwi
void animateTurtles()
{
	// zwykle zolwie
	if ((passTime - int(passTime) >= 0.0 && passTime - int(passTime) < 0.33))
	{
		for (int i = 0; i < TURTLE1_NUMBER; i++)
			turtle1[i].sprite = bturtle1_1;
		for (int i = 0; i < TURTLE2_NUMBER; i++)
			turtle2[i].sprite = bturtle2_1;
	}
	else if ((passTime - int(passTime) >= 0.33 && passTime - int(passTime) < 0.66))
	{
		for (int i = 0; i < TURTLE1_NUMBER; i++)
			turtle1[i].sprite = bturtle1_2;
		for (int i = 0; i < TURTLE2_NUMBER; i++)
			turtle2[i].sprite = bturtle2_2;
	}
	else
	{
		for (int i = 0; i < TURTLE1_NUMBER; i++)
			turtle1[i].sprite = bturtle1_3;
		for (int i = 0; i < TURTLE2_NUMBER; i++)
			turtle2[i].sprite = bturtle2_3;
	}

	// zolwie nurkujace
	if (int(passTime) % 3 == 0)
	{
		if ((passTime - int(passTime) >= 0.0 && passTime - int(passTime) < 0.33))
		{
			for (int i = 0; i < TURTLE1DIVE_NUMBER; i++)
				turtle1dive[i].sprite = bturtle1_1;
			for (int i = 0; i < TURTLE2DIVE_NUMBER; i++)
				turtle2dive[i].sprite = bturtle2_4;
		}
		else if ((passTime - int(passTime) >= 0.33 && passTime - int(passTime) < 0.66))
		{
			for (int i = 0; i < TURTLE1DIVE_NUMBER; i++)
				turtle1dive[i].sprite = bturtle1_2;
			for (int i = 0; i < TURTLE2DIVE_NUMBER; i++)
				turtle2dive[i].sprite = bturtle2_5;
		}
		else
		{
			for (int i = 0; i < TURTLE1DIVE_NUMBER; i++)
				turtle1dive[i].sprite = bturtle1_3;
			for (int i = 0; i < TURTLE2DIVE_NUMBER; i++)
				turtle2dive[i].sprite = bturtle2_6;
		}
	}
	else if (int(passTime) % 3 == 1)
	{
		if ((passTime - int(passTime) >= 0.0 && passTime - int(passTime) < 0.33))
		{
			for (int i = 0; i < TURTLE1DIVE_NUMBER; i++)
				turtle1dive[i].sprite = bturtle1_4;
			for (int i = 0; i < TURTLE2DIVE_NUMBER; i++)
				turtle2dive[i].sprite = bturtle2_6;
		}
		else if ((passTime - int(passTime) >= 0.33 && passTime - int(passTime) < 0.66))
		{
			for (int i = 0; i < TURTLE1DIVE_NUMBER; i++)
				turtle1dive[i].sprite = bturtle1_5;
			for (int i = 0; i < TURTLE2DIVE_NUMBER; i++)
				turtle2dive[i].sprite = bturtle2_5;
		}
		else
		{
			for (int i = 0; i < TURTLE1DIVE_NUMBER; i++)
				turtle1dive[i].sprite = bturtle1_6;
			for (int i = 0; i < TURTLE2DIVE_NUMBER; i++)
				turtle2dive[i].sprite = bturtle2_4;
		}
	}
	else if (int(passTime) % 3 == 2)
	{
		if ((passTime - int(passTime) >= 0.0 && passTime - int(passTime) < 0.33))
		{
			for (int i = 0; i < TURTLE1DIVE_NUMBER; i++)
				turtle1dive[i].sprite = bturtle1_6;
			for (int i = 0; i < TURTLE2DIVE_NUMBER; i++)
				turtle2dive[i].sprite = bturtle2_1;
		}
		else if ((passTime - int(passTime) >= 0.33 && passTime - int(passTime) < 0.66))
		{
			for (int i = 0; i < TURTLE1DIVE_NUMBER; i++)
				turtle1dive[i].sprite = bturtle1_5;
			for (int i = 0; i < TURTLE2DIVE_NUMBER; i++)
				turtle2dive[i].sprite = bturtle2_2;
		}
		else
		{
			for (int i = 0; i < TURTLE1DIVE_NUMBER; i++)
				turtle1dive[i].sprite = bturtle1_4;
			for (int i = 0; i < TURTLE2DIVE_NUMBER; i++)
				turtle2dive[i].sprite = bturtle2_3;
		}
	}
}


// bada kolizje gracza ze zwyklym samochodem
bool checkCollisonCarFixed(struct frog hero, struct car car)
{
	if ((car.pos.x + (((car.sprite->w) - 4) * 3) - 1 < hero.pos.x) || (car.pos.x + 1 > hero.pos.x + (((hero.sprite->w) - 4) * 3)) || (car.pos.y + ((car.sprite->h) * 3) - 1 < hero.pos.y) || (car.pos.y + 1 > hero.pos.y + ((hero.sprite->h) * 3)))
	{
		return false;
	}
	return true;
}


// bada kolizje gracza z ciezarowka
bool checkCollisonTruckFixed(struct frog hero, struct car car)
{
	if ((car.pos.x + (((car.sprite->w) - 8) * 3) - 1 < hero.pos.x) || (car.pos.x + 1 > hero.pos.x + (((hero.sprite->w) - 6) * 3)) || (car.pos.y + ((car.sprite->h) * 3) - 1 < hero.pos.y) || (car.pos.y + 1 > hero.pos.y + ((hero.sprite->h) * 3)))
	{
		return false;
	}
	return true;
}


// bada kolizje gracza z zolwiem
bool checkCollisonTurtleFixed(struct frog hero, struct car car)
{
	if ((car.pos.x + (((car.sprite->w) - 5) * 3) - 1 < hero.pos.x) || (car.pos.x + 1 > hero.pos.x + (((hero.sprite->w) - 5) * 3)) || (car.pos.y + ((car.sprite->h) * 3) - 1 < hero.pos.y) || (car.pos.y + 1 > hero.pos.y + ((hero.sprite->h) * 3)))
	{
		return false;
	}
	return true;
}


// bada kolizje gracza z drewienkiem
bool checkCollisonLogFixed(struct frog hero, struct car car)
{
	if ((car.pos.x + (((car.sprite->w) - 7) * 3) - 1 < hero.pos.x) || (car.pos.x + 1 > hero.pos.x + (((hero.sprite->w) - 7) * 3)) || (car.pos.y + ((car.sprite->h) * 3) - 1 < hero.pos.y) || (car.pos.y + 1 > hero.pos.y + ((hero.sprite->h) * 3)))
	{
		return false;
	}
	return true;
}


// sprawdza czy samochod potracil gracza
bool heroCollidesWithCar()
{
	//car1
	for (int i = 0; i < CAR1_NUMBER; i++)
	{
		if (checkCollisonCarFixed(hero, car1[i]))
			return true;
	}
	//car2
	for (int i = 0; i < CAR2_NUMBER; i++)
	{
		if (checkCollisonCarFixed(hero, car2[i]))
			return true;
	}
	//car3
	for (int i = 0; i < CAR3_NUMBER; i++)
	{
		if (checkCollisonCarFixed(hero, car3[i]))
			return true;
	}
	//car4
	for (int i = 0; i < CAR4_NUMBER; i++)
	{
		if (checkCollisonCarFixed(hero, car4[i]))
			return true;
	}
	//car5
	for (int i = 0; i < CAR5_NUMBER; i++)
	{
		if (checkCollisonTruckFixed(hero, car5[i]))
			return true;
	}

	return false;
}


// funkcje odpowiedzialne za sprawdzenie kolizji gracza z "platformami"
bool heroCollidesWithTurtle1()
{
	//turtle1
	for (int i = 0; i < TURTLE1_NUMBER; i++)
	{
		if (checkCollisonTurtleFixed(hero, turtle1[i]))
			return true;
	}

	return false;
}
bool heroCollidesWithTurtle1dive()
{
	//turtle1dive
	for (int i = 0; i < TURTLE1DIVE_NUMBER; i++)
	{
		if (checkCollisonTurtleFixed(hero, turtle1dive[i]) && turtle1dive[i].sprite != bturtle1_6)
			return true;
	}

	return false;
}
bool heroCollidesWithLog1()
{
	//log1
	for (int i = 0; i < LOG1_NUMBER; i++)
	{
		if (checkCollisonLogFixed(hero, logg1[i]))
			return true;
	}

	return false;
}
bool heroCollidesWithLog2()
{
	//log2
	for (int i = 0; i < LOG2_NUMBER; i++)
	{
		if (checkCollisonLogFixed(hero, logg2[i]))
			return true;
	}

	return false;
}
bool heroCollidesWithTurtle2()
{
	//turtle2
	for (int i = 0; i < TURTLE2_NUMBER; i++)
	{
		if (checkCollisonTurtleFixed(hero, turtle2[i]))
			return true;
	}

	return false;
}
bool heroCollidesWithTurtle2dive()
{
	//turtle2dive
	for (int i = 0; i < TURTLE2DIVE_NUMBER; i++)
	{
		if (checkCollisonTurtleFixed(hero, turtle2dive[i]) && turtle2dive[i].sprite != bturtle2_6)
			return true;
	}

	return false;
}
bool heroCollidesWithLog3()
{
	//log3
	for (int i = 0; i < LOG3_NUMBER; i++)
	{
		if (checkCollisonLogFixed(hero, logg3[i]))
			return true;
	}

	return false;
}


// sprawdza czy gracz jest na "platformie"
bool heroCollidesWithFriend()
{
	if (heroCollidesWithTurtle1()) return true;
	if (heroCollidesWithTurtle1dive()) return true;
	if (heroCollidesWithLog1()) return true;
	if (heroCollidesWithLog2()) return true;
	if (heroCollidesWithTurtle2()) return true;
	if (heroCollidesWithTurtle2dive()) return true;
	if (heroCollidesWithLog3()) return true;
	return false;
}


// sprawdza czy gracz jest na wodzie, i jezeli nie ma go na platformie to tonie
void checkWater()
{
	if (hero.pos.y >= 144 && hero.pos.y < 384)
	{
		if (heroCollidesWithFriend() && active == 1)
		{
			if (heroCollidesWithTurtle1())
			{
				if (turtle1[0].dir == car::direction::LEFT && hero.pos.y == turtle1[0].pos.y)
				{
					hero.pos.x -= turtle1[0].speed;
				}
				else if (turtle1[0].dir == car::direction::RIGHT && hero.pos.y == turtle1[0].pos.y)
				{
					hero.pos.x += turtle1[0].speed;
				}
			}
			else if (heroCollidesWithTurtle1dive())
			{
				if (turtle1dive[0].dir == car::direction::LEFT && hero.pos.y == turtle1dive[0].pos.y)
				{
					hero.pos.x -= turtle1dive[0].speed;
				}
				else if (turtle1dive[0].dir == car::direction::RIGHT && hero.pos.y == turtle1dive[0].pos.y)
				{
					hero.pos.x += turtle1dive[0].speed;
				}
			}
			else if (heroCollidesWithLog1())
			{
				if (logg1[0].dir == car::direction::LEFT && hero.pos.y == logg1[0].pos.y)
				{
					hero.pos.x -= logg1[0].speed;
				}
				else if (logg1[0].dir == car::direction::RIGHT && hero.pos.y == logg1[0].pos.y)
				{
					hero.pos.x += logg1[0].speed;
				}
			}
			else if (heroCollidesWithLog2())
			{
				if (logg2[0].dir == car::direction::LEFT && hero.pos.y == logg2[0].pos.y)
				{
					hero.pos.x -= logg2[0].speed;
				}
				else if (logg2[0].dir == car::direction::RIGHT && hero.pos.y == logg2[0].pos.y)
				{
					hero.pos.x += logg2[0].speed;
				}
			}
			else if (heroCollidesWithTurtle2())
			{
				if (turtle2[0].dir == car::direction::LEFT && hero.pos.y == turtle2[0].pos.y)
				{
					hero.pos.x -= turtle2[0].speed;
				}
				else if (turtle2[0].dir == car::direction::RIGHT && hero.pos.y == turtle2[0].pos.y)
				{
					hero.pos.x += turtle2[0].speed;
				}
			}
			else if (heroCollidesWithTurtle2dive())
			{
				if (turtle2dive[0].dir == car::direction::LEFT && hero.pos.y == turtle2dive[0].pos.y)
				{
					hero.pos.x -= turtle2dive[0].speed;
				}
				else if (turtle2dive[0].dir == car::direction::RIGHT && hero.pos.y == turtle2dive[0].pos.y)
				{
					hero.pos.x += turtle2dive[0].speed;
				}
			}
			else if (heroCollidesWithLog3())
			{
				if (logg3[0].dir == car::direction::LEFT && hero.pos.y == logg3[0].pos.y)
				{
					hero.pos.x -= logg3[0].speed;
				}
				else if (logg3[0].dir == car::direction::RIGHT && hero.pos.y == logg3[0].pos.y)
				{
					hero.pos.x += logg3[0].speed;
				}
			}
		}
		else
		{
			die(1);
		}
	}
}


// sprawdza czy gracz wyplynal poza ekran, jesli tak to ginie
void checkBounds()
{
	if (hero.pos.x < 0 || hero.pos.x > SCREEN_WIDTH - 48)
		die(1);
}


// sprawdza czy gracz idzie w gore w celu przyznania punktow
void checkMovingUp()
{
	if (hero.pos.y == 624 && movingUp[0] == 0)
	{
		movingUp[0] = 1;
		score += 10;
	}
	if (hero.pos.y == 576 && movingUp[1] == 0)
	{
		movingUp[1] = 1;
		score += 10;
	}
	if (hero.pos.y == 528 && movingUp[2] == 0)
	{
		movingUp[2] = 1;
		score += 10;
	}
	if (hero.pos.y == 480 && movingUp[3] == 0)
	{
		movingUp[3] = 1;
		score += 10;
	}
	if (hero.pos.y == 432 && movingUp[4] == 0)
	{
		movingUp[4] = 1;
		score += 10;
	}
	if (hero.pos.y == 384 && movingUp[5] == 0)
	{
		movingUp[5] = 1;
		score += 10;
	}
	if (hero.pos.y == 336 && movingUp[6] == 0)
	{
		movingUp[6] = 1;
		score += 10;
	}
	if (hero.pos.y == 288 && movingUp[7] == 0)
	{
		movingUp[7] = 1;
		score += 10;
	}
	if (hero.pos.y == 240 && movingUp[8] == 0)
	{
		movingUp[8] = 1;
		score += 10;
	}
	if (hero.pos.y == 192 && movingUp[9] == 0)
	{
		movingUp[9] = 1;
		score += 10;
	}
	if (hero.pos.y == 144 && movingUp[10] == 0)
	{
		movingUp[10] = 1;
		score += 10;
	}
	if (hero.pos.y == 96 && movingUp[11] == 0)
	{
		movingUp[11] = 1;
		score += 10;
	}
}


// daje znac ze bedzie wypisywac czas w jakim udalo sie doprowadzic zabke na brzeg
void setTimeRect()
{
	timeRect = 192;
}


// wyswietla czas w jakim gracz doprowadzil zabke na brzeg
void drawTimeRect()
{
	if (timeRect > 0)
	{
		if (!loadRedCharset()) { printf("loadRedCharset error\n"); return; }

		if (timeCopy >= 10)
		{
			DrawRectangle(screen, 444, 390, 216, 36, black);
			sprintf(text, "TIME %d", timeCopy); DrawString(screen, 252, 396, text, charset);
		}
		else
		{
			DrawRectangle(screen, 432, 390, 192, 36, black);
			sprintf(text, "TIME %d", timeCopy); DrawString(screen, 264, 396, text, charset);
		}

		timeRect--;
	}
}


// resetuje czas, przemieszcza gracza na start i dodaje punkty po uratowaniu zabki
void frogSaved()
{
	active = 0;
	happySaved++;
	moveHeroAtStart();
	score += 50;
	timeCopy = int(50 - worldTime);
	score += 10 * timeCopy;
	if (happySaved < 5)
	{
		setTimeRect();
		worldTime = 0;
	}
	active = 1;
}


// sprawdza czy gracz doprowadzil zabke na brzeg
void checkHappy()
{
	if (hero.pos.y < 144)
	{
		if (hero.pos.x >= 9 && hero.pos.x < 39 && happy[0] == 0)
		{
			if (hero.pos.y <= 96)
			{
				happy[0] = 1;
				frogSaved();
			}
		}
		else if (hero.pos.x >= 153 && hero.pos.x < 183 && happy[1] == 0)
		{
			if (hero.pos.y <= 96)
			{
				happy[1] = 1;
				frogSaved();
			}
		}
		else if (hero.pos.x >= 297 && hero.pos.x < 327 && happy[2] == 0)
		{
			if (hero.pos.y <= 96)
			{
				happy[2] = 1;
				frogSaved();
			}
		}
		else if (hero.pos.x >= 441 && hero.pos.x < 471 && happy[3] == 0)
		{
			if (hero.pos.y <= 96)
			{
				happy[3] = 1;
				frogSaved();
			}
		}
		else if (hero.pos.x >= 585 && hero.pos.x < 615 && happy[4] == 0)
		{
			if (hero.pos.y <= 96)
			{
				happy[4] = 1;
				frogSaved();
			}
		}
		else
		{
			die(0);
		}
	}
}


// rysuje doprowadzone zabki na ekranie
void drawHappy()
{
	if (happy[0] == 1) DrawSurface(screen, bhappyfrog, 24, 96);
	if (happy[1] == 1) DrawSurface(screen, bhappyfrog, 168, 96);
	if (happy[2] == 1) DrawSurface(screen, bhappyfrog, 312, 96);
	if (happy[3] == 1) DrawSurface(screen, bhappyfrog, 456, 96);
	if (happy[4] == 1) DrawSurface(screen, bhappyfrog, 600, 96);
}


// sprawdza czy gracz doprowadzil wszystkie zabki na brzeg
bool allFrogsSaved()
{
	for (int i = 0; i < 5; i++)
	{
		if (!happy[i]) return false;
	}
	return true;
}


// liczy czas, fps, klatki
void countTime()
{
	t2 = SDL_GetTicks();
	delta = (t2 - t1) * 0.001;
	t1 = t2;
	if (active == 1 && over == 0 && pause == 0 && wantsToQuit == 0) worldTime += delta;
	passTime += delta;

	if (over == 0 && pause == 0 && wantsToQuit == 0)
	{
		fpsTimer += delta;
		if (fpsTimer > 0.5)
		{
			fps = frames * 2;
			frames = 0;
			fpsTimer -= 0.5;
		}
	}
}


// daje znac ze zaba bedzie poruszala sie w lewo
void setMoveLeft()
{
	hero.dir = frog::direction::LEFT;
	hero.sprite = bfrog1a1;
	moving = 48;
}


// daje znac ze zaba bedzie poruszala sie w prawo
void setMoveRight()
{
	hero.dir = frog::direction::RIGHT;
	hero.sprite = bfrog1d1;
	moving = 48;
}


// daje znac ze zaba bedzie poruszala sie w gore
void setMoveUp()
{
	hero.dir = frog::direction::UP;
	hero.sprite = bfrog1w1;
	moving = 48;
}


// daje znac ze zaba bedzie poruszala sie w dol
void setMoveDown()
{
	hero.dir = frog::direction::DOWN;
	hero.sprite = bfrog1s1;
	moving = 48;
}


// rusza w lewo
void moveLeft()
{
	hero.pos.x -= 3;
	moving -= 3;
	if (moving <= 40 && moving > 10) hero.sprite = bfrog1a2;
	else hero.sprite = bfrog1a1;
}


// rusza w prawo
void moveRight()
{
	hero.pos.x += 3;
	moving -= 3;
	if (moving <= 40 && moving > 10) hero.sprite = bfrog1d2;
	else hero.sprite = bfrog1d1;
}


// rusza w gore
void moveUp()
{
	hero.pos.y -= 3;
	moving -= 3;
	if (moving <= 40 && moving > 10) hero.sprite = bfrog1w2;
	else hero.sprite = bfrog1w1;
}


// rusza w dol
void moveDown()
{
	hero.pos.y += 3;
	moving -= 3;
	if (moving <= 40 && moving > 10) hero.sprite = bfrog1s2;
	else hero.sprite = bfrog1s1;
}


// sprawdza czy zaba bedzie sie poruszala: jesli tak, umozliwia ten ruch
void move()
{
	if (moving > 0)
	{
		if (hero.dir == frog::direction::LEFT)
		{
			moveLeft();
		}
		else if (hero.dir == frog::direction::RIGHT)
		{
			moveRight();
		}
		else if (hero.dir == frog::direction::UP)
		{
			moveUp();
		}
		else if (hero.dir == frog::direction::DOWN)
		{
			moveDown();
		}
	}
}


// wypisuje na ekran "paused"
void drawPause()
{
	DrawRectangle(screen, 432, 390, 192, 36, black);
	if (!loadYellowCharset()) { printf("loadYellowCharset error\n"); return; }
	sprintf(text, "PAUSED"); DrawString(screen, 264, 396, text, charset);
}


// wypisuje na ekran "quit game? y/n"
void drawQuit()
{
	DrawRectangle(screen, 528, 390, 384, 36, black);
	if (!loadRedCharset()) { printf("loadRedCharset error\n"); return; }
	sprintf(text, "QUIT GAME? Y/N"); DrawString(screen, 168, 396, text, charset);
}


// rysuje mape
void drawMap()
{
	DrawSurface(screen, bmap, 0, 0);
}


// rysuje bohatera
void drawHero()
{
	if (over == 0) DrawSurface(screen, hero.sprite, hero.pos.x, hero.pos.y);
}


// wypisuje na ekran "game over", na zolto jesli n rowne 1, na czerwono jesli n rowne 0
void drawOver(int n)
{
	if (overAnim > 0)
	{
		DrawRectangle(screen, 468, 390, 264, 36, black);
		if (n == 0)
		{
			if (!loadRedCharset()) { printf("loadRedCharset error\n"); return; }
		}
		else if (n == 1)
		{
			if (!loadYellowCharset()) { printf("loadYellowCharset error\n"); return; }
		}
		sprintf(text, "GAME OVER"); DrawString(screen, 228, 396, text, charset);

		overAnim--;
	}
}


// rysuje hud
void drawHud()
{
	if (!loadGrayCharset()) { printf("loadGrayCharset error\n"); return; }
	sprintf(text, "SCORE"); DrawString(screen, 72, 0, text, charset);
	sprintf(text, "HI-SCORE"); DrawString(screen, 240, 0, text, charset);
	sprintf(text, "FPS"); DrawString(screen, 480, 0, text, charset);

	if (!loadRedCharset()) { printf("loadRedCharset error\n"); return; }
	//score
	if (score >= 99990)
	{
		sprintf(text, "99990");
		DrawString(screen, 72, 24, text, charset);
	}
	else
	{
		sprintf(text, "%05d", score);
		DrawString(screen, 72, 24, text, charset);
	}
	//hiscore
	if (score >= highestScore) highestScore = score;

	if (highestScore >= 99990)
	{
		sprintf(text, "99990");
		DrawString(screen, 264, 24, text, charset);
	}
	else
	{
		sprintf(text, "%05d", highestScore);
		DrawString(screen, 264, 24, text, charset);
	}
	sprintf(text, "%.0lf", fps); DrawString(screen, 480, 24, text, charset);

	drawLives();
	DrawSurface(screen, blevel, 624, 720);
	drawTime();

	drawTimeRect();
	if (over == 1 && done == 0) drawOver(0);
	if (over == 1 && done == 1) drawOver(1);
	if (pause) drawPause();
	if (wantsToQuit) drawQuit();
	if (over == 1 && overAnim == 0 && typed == 0) drawEnterName();
}


// rysuje wszystko poza hudem
void drawEverythingButHud()
{
	if (pause == 0 && wantsToQuit == 0)
	{
		drawMap();
		drawMovingObjects();
		drawHappy();
		drawHero();
	}
}


// sprawdza czy ukonczono gre
void checkIfFinished()
{
	if (!done)
	{
		if (allFrogsSaved())
		{
			if (over == 0 && done == 0) score += 1000;
			over = 1;
			done = 1;
			overAnim = 192;
		}
	}
}


// sprawdza czy potracono zabke lub 
void handleDeath()
{
	if (heroCollidesWithCar()) die(0);
	if (50 - worldTime <= 0) die(0);
}


// sprawdza czy wprowadzony znak jest litera
bool checkLetter()
{
	bool success = false;

	// litery
	if ((event.key.keysym.sym == SDLK_a)) success = true;
	if ((event.key.keysym.sym == SDLK_b)) success = true;
	if ((event.key.keysym.sym == SDLK_c)) success = true;
	if ((event.key.keysym.sym == SDLK_d)) success = true;
	if ((event.key.keysym.sym == SDLK_e)) success = true;
	if ((event.key.keysym.sym == SDLK_f)) success = true;
	if ((event.key.keysym.sym == SDLK_g)) success = true;
	if ((event.key.keysym.sym == SDLK_h)) success = true;
	if ((event.key.keysym.sym == SDLK_i)) success = true;
	if ((event.key.keysym.sym == SDLK_j)) success = true;
	if ((event.key.keysym.sym == SDLK_k)) success = true;
	if ((event.key.keysym.sym == SDLK_l)) success = true;
	if ((event.key.keysym.sym == SDLK_m)) success = true;
	if ((event.key.keysym.sym == SDLK_n)) success = true;
	if ((event.key.keysym.sym == SDLK_o)) success = true;
	if ((event.key.keysym.sym == SDLK_p)) success = true;
	if ((event.key.keysym.sym == SDLK_q)) success = true;
	if ((event.key.keysym.sym == SDLK_r)) success = true;
	if ((event.key.keysym.sym == SDLK_s)) success = true;
	if ((event.key.keysym.sym == SDLK_t)) success = true;
	if ((event.key.keysym.sym == SDLK_u)) success = true;
	if ((event.key.keysym.sym == SDLK_v)) success = true;
	if ((event.key.keysym.sym == SDLK_w)) success = true;
	if ((event.key.keysym.sym == SDLK_x)) success = true;
	if ((event.key.keysym.sym == SDLK_y)) success = true;
	if ((event.key.keysym.sym == SDLK_z)) success = true;

	return success;
}


// sprawdza czy wprowadzony znak jest cyfra czy spacja
bool checkChar()
{
	bool success = false;

	// cyfry 
	if ((event.key.keysym.sym == SDLK_0)) success = true;
	if ((event.key.keysym.sym == SDLK_1)) success = true;
	if ((event.key.keysym.sym == SDLK_2)) success = true;
	if ((event.key.keysym.sym == SDLK_3)) success = true;
	if ((event.key.keysym.sym == SDLK_4)) success = true;
	if ((event.key.keysym.sym == SDLK_5)) success = true;
	if ((event.key.keysym.sym == SDLK_6)) success = true;
	if ((event.key.keysym.sym == SDLK_7)) success = true;
	if ((event.key.keysym.sym == SDLK_8)) success = true;
	if ((event.key.keysym.sym == SDLK_9)) success = true;

	// cyfry keypad
	if ((event.key.keysym.sym == SDLK_KP_0)) { event.key.keysym.sym = SDLK_0; success = true; }
	if ((event.key.keysym.sym == SDLK_KP_1)) { event.key.keysym.sym = SDLK_1; success = true; }
	if ((event.key.keysym.sym == SDLK_KP_2)) { event.key.keysym.sym = SDLK_2; success = true; }
	if ((event.key.keysym.sym == SDLK_KP_3)) { event.key.keysym.sym = SDLK_3; success = true; }
	if ((event.key.keysym.sym == SDLK_KP_4)) { event.key.keysym.sym = SDLK_4; success = true; }
	if ((event.key.keysym.sym == SDLK_KP_5)) { event.key.keysym.sym = SDLK_5; success = true; }
	if ((event.key.keysym.sym == SDLK_KP_6)) { event.key.keysym.sym = SDLK_6; success = true; }
	if ((event.key.keysym.sym == SDLK_KP_7)) { event.key.keysym.sym = SDLK_7; success = true; }
	if ((event.key.keysym.sym == SDLK_KP_8)) { event.key.keysym.sym = SDLK_8; success = true; }
	if ((event.key.keysym.sym == SDLK_KP_9)) { event.key.keysym.sym = SDLK_9; success = true; }

	// backspace
	if ((event.key.keysym.sym == SDLK_BACKSPACE)) success = true;
	if ((event.key.keysym.sym == SDLK_KP_BACKSPACE)) { event.key.keysym.sym = SDLK_BACKSPACE; success = true; }

	// litery
	if (checkLetter()) success = true;

	return success;
}


// obsluguje klawiature
void handleInput()
{
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			quit = 1;
		}

		else if (event.type == SDL_KEYDOWN)
		{
			if (over == 1 && overAnim == 0 && typed == 0)
			{
				if ((event.key.keysym.sym == SDLK_RETURN) && strlen(name))
				{
					typed = 1;

					for (int i = 0; i < 10; i++)
					{
						if (score >= scores[i])
						{
							moveDownTable(i);

							typedPos = i;

							if (score >= 99990)
							{
								scores[i] = 99990;
							}
							else
							{
								scores[i] = score;
							}

							if (i == 0) highestScore = score;
							zamien(names[i], name);
							saveHighscores();

							break;
						}
					}

					gameFrame = 2;
				}
				else
				{
					if ((event.key.keysym.sym != SDLK_F4) && checkChar())
					{
						if (checkLetter() && strlen(name) < 11)
						{
							name[strlen(name)] = event.key.keysym.sym - 32;
						}
						else if (!(event.key.keysym.sym == SDLK_BACKSPACE) && strlen(name) < 11)
						{
							name[strlen(name)] = event.key.keysym.sym;
						}
						else if ((event.key.keysym.sym == SDLK_BACKSPACE) && strlen(name) > 0)
						{
							name[strlen(name) - 1] = '\0';
						}
					}
				}
			}
			if ((event.key.keysym.sym == SDLK_F4))
			{
				setFullScreen();
			}
			if ((event.key.keysym.sym == SDLK_F5))
			{
				score += 1000;
			}
			if ((event.key.keysym.sym == SDLK_p) && wantsToQuit == 0 && over == 0)
			{
				pause = !pause;
			}
			if ((event.key.keysym.sym == SDLK_q) && pause == 0 && over == 0)
			{
				wantsToQuit = 1;
			}
			if ((event.key.keysym.sym == SDLK_y) && wantsToQuit == 1)
			{
				gameFrame = 0;
			}
			if ((event.key.keysym.sym == SDLK_n) && wantsToQuit == 1)
			{
				wantsToQuit = 0;
			}
			//sterowanie
			if ((event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) && hero.pos.x > 0 && active == 1 && moving == 0 && dying == 0 && over == 0 && pause == 0 && wantsToQuit == 0)
			{
				setMoveLeft();
			}
			if ((event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d) && hero.pos.x < 624 && active == 1 && moving == 0 && dying == 0 && over == 0 && pause == 0 && wantsToQuit == 0)
			{
				setMoveRight();
			}
			if ((event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) && hero.pos.y > 96 && active == 1 && moving == 0 && dying == 0 && over == 0 && pause == 0 && wantsToQuit == 0)
			{
				setMoveUp();
			}
			if ((event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) && hero.pos.y < 672 && active == 1 && moving == 0 && dying == 0 && over == 0 && pause == 0 && wantsToQuit == 0)
			{
				setMoveDown();
			}
		}
	}
}


// obsluga wszystkich zdarzen 
void events()
{
	handleInput();
	if (pause == 0 && wantsToQuit == 0)
	{
		checkDying();
		move();
		handleDeath();
		checkIfFinished();
		checkMovingUp();
		checkWater();
		checkBounds();
		checkHappy();
		animateCars();
		animateTurtles();
		moveMovingObjects();
	}
}


// rysuje obiekty na ekranie, aktualizuje czas i rysuje hud
void draw()
{
	drawEverythingButHud();
	countTime();
	drawHud();
}


// renderuje obiekty na ekran
void render()
{
	if (quit == 0)
	{
		SDL_RenderClear(renderer);
		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);
	}
}


// rysuje opcje w menu
void drawMenuOptions()
{
	switch (selectedMenu)
	{
	case 0:
	{
		DrawSurface(screen, bfrog2w1, 192, 327);
		DrawSurface(screen, bfrog2w1, 432, 327);

		loadTurquoiseCharset();
		sprintf(text, "NEW GAME"); DrawString(screen, 240, 336, text, charset);

		loadVioletCharset();
		sprintf(text, "HI-SCORES"); DrawString(screen, 228, 408, text, charset);
		sprintf(text, "QUIT FROGGER"); DrawString(screen, 192, 480, text, charset);

		break;
	}
	case 1:
	{
		DrawSurface(screen, bfrog2w1, 180, 399);
		DrawSurface(screen, bfrog2w1, 444, 399);

		loadTurquoiseCharset();
		sprintf(text, "HI-SCORES"); DrawString(screen, 228, 408, text, charset);

		loadVioletCharset();
		sprintf(text, "NEW GAME"); DrawString(screen, 240, 336, text, charset);
		sprintf(text, "QUIT FROGGER"); DrawString(screen, 192, 480, text, charset);

		break;
	}
	case 2:
	{
		DrawSurface(screen, bfrog2w1, 144, 471);
		DrawSurface(screen, bfrog2w1, 480, 471);

		loadTurquoiseCharset();
		sprintf(text, "QUIT FROGGER"); DrawString(screen, 192, 480, text, charset);

		loadVioletCharset();
		sprintf(text, "NEW GAME"); DrawString(screen, 240, 336, text, charset);
		sprintf(text, "HI-SCORES"); DrawString(screen, 228, 408, text, charset);

		break;
	}
	}
}


// rysuje opcje w menu
void drawMenu()
{
	DrawSurface(screen, bback, 0, 0);
	DrawSurface(screen, blogotitle, 96, 144);
	drawMenuOptions();
	loadTurquoiseCharset();
	sprintf(text, "MARCIN CHETNIK"); DrawString(screen, 0, 744, text, charset);
	text[0] = 127; text[1] = NULL; DrawString(screen, 552, 744, text, charset);
	sprintf(text, "2020"); DrawString(screen, 576, 744, text, charset);
}


// wykonuje czynnosci w zaleznosci od wybranej opcji w menu
void playerSelectedOption()
{
	switch (selectedMenu)
	{
	case 0:
	{
		gameFrame = 1;
		break;
	}
	case 1:
	{
		gameFrame = 2;
		break;
	}
	case 2:
	{
		quit = 1;
		break;
	}
	}
}


// resetuje imie gracza
void resetName()
{
	for (int i = strlen(name) - 1; i >= 0; i--)
	{
		name[i] = '\0';
	}
}


// resetuje statystyki w menu glownym
void resetFirstStats()
{
	lives = LIVES_NUMBER;
	done = 0;
	over = 0;
	score = 0;
	selectedMenu = 0;
	typed = 0;
	typedPos = 10;
	resetName();
}


// resetuje statystyki w highscores
void resetHighScores()
{
	done = 0;
	over = 0;
}


// rysuje ostatni gameFrame
void drawLastGameFrame()
{
	DrawSurface(screen, bback, 0, 0);
	drawHighScores();
}


// obsluguje zdarzenia w menu
void eventsMenu()
{
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			quit = 1;
		}

		else if (event.type == SDL_KEYDOWN)
		{
			if ((event.key.keysym.sym == SDLK_RETURN))
			{
				playerSelectedOption();
			}
			if ((event.key.keysym.sym == SDLK_F4))
			{
				setFullScreen();
			}
			if ((event.key.keysym.sym == SDLK_DOWN))
			{
				selectedMenu++;
				if (selectedMenu > 2) selectedMenu = 0;
			}
			if ((event.key.keysym.sym == SDLK_UP))
			{
				selectedMenu--;
				if (selectedMenu < 0) selectedMenu = 2;
			}
		}
	}
}


// obsluguje zdarzenia w highscores
void eventsHighscores()
{
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			quit = 1;
		}

		else if (event.type == SDL_KEYDOWN)
		{
			if ((event.key.keysym.sym == SDLK_RETURN) || (event.key.keysym.sym == SDLK_ESCAPE))
			{
				gameFrame = 0;
			}
			if ((event.key.keysym.sym == SDLK_F4))
			{
				setFullScreen();
			}
		}
	}
}


// obsluguje w calosci gameFrame = 0
void manageFrame0()
{
	resetFirstStats();
	while (gameFrame == 0 && !quit)
	{
		eventsMenu();
		drawMenu();
		render();
	}
}


// obsluguje w calosci gameFrame = 1
void manageFrame1()
{
	startTime();
	while (gameFrame == 1 && !quit)
	{
		events();
		draw();
		render();
		frames++;
	}
}


// obsluguje w calosci gameFrame = 2
void manageFrame2()
{
	resetHighScores();
	while (gameFrame == 2 && !quit)
	{
		eventsHighscores();
		drawLastGameFrame();
		render();
	}
}


// tworzy gre i jej cala logike
void froggerGame()
{
	if (!init())
	{

		return;
	}

	if (!load())
	{

		return;
	}

	start();

	while (!quit)
	{
		if (gameFrame == 0)
		{
			manageFrame0();
		}
		if (gameFrame == 1)
		{
			manageFrame1();
		}
		if (gameFrame == 2)
		{
			manageFrame2();
		}
	}

	free();
}