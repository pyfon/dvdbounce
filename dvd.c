/* This is my own attempt at making the DVD bounce animation. */
/* Written by Nathan Weetman, 15th April 2020 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480
#define LOGO_WIDTH  128
#define LOGO_HEIGHT 76
#define LOGO_PNG "png/dvd.svg.png"

/* Globals: */
SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;
SDL_Texture *logoTexture = NULL;

/* Functions */

void init();
void loadMedia();
void closeSDL(int);
void fatalError(const char*, const char*);
void changeColour(SDL_Texture*);
SDL_Texture* loadTexture(char *path);

/* Starts SDL and creates window */
void init(){

	/* Initalise SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		fatalError("SDL_Init() failed", SDL_GetError());
	/* Set texture filtering to linear */
	if (! SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		printf("Warning: Linear texture filtering disabled.\n");
	/* Create window */
	gWindow = SDL_CreateWindow("Nathan's DVD Bounce", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (gWindow == NULL)
		fatalError("SDL_CreateWindow() failed", SDL_GetError());
	/* Create renderer for window */
	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (gRenderer == NULL)
		fatalError("SDL_CreateRenderer failed", SDL_GetError());
	/* Initialise renderer colour */
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
	/* Initialise PNG loading */
	int imgFlags = IMG_INIT_PNG;
	if (! (IMG_Init(imgFlags) & imgFlags) )
		fatalError("IMG_Init failed", IMG_GetError());

}

/* Loads media */
void loadMedia(){

	logoTexture = loadTexture(LOGO_PNG);
	if (logoTexture == NULL)
		fatalError("Failed to load texture image", NULL);

}

void closeSDL(int status){

	SDL_DestroyTexture(logoTexture);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;
	/* Quit SDL subsystems */
	IMG_Quit();
	SDL_Quit();
	exit(status);

}

void fatalError(const char *msg, const char *sdlErr){

	printf("ERROR:\n%s: %s\n", msg, sdlErr);
	closeSDL(EXIT_FAILURE);

}

void changeColour(SDL_Texture *tex){

	/* Choose a random colour and change tex */

	static int prev = 0;

	struct colour {
		Uint8 red, green, blue;
	} colours[] = {
		{0xFF, 0xFF, 0xFF}, /* White */
		{0xFF, 0x00, 0x00}, /* Red */
		{0xFF, 0xFF, 0x00}, /* Yellow */
		{0x00, 0xFF, 0x00}, /* Green */
		{0x00, 0xFF, 0xFF}, /* Cyan */
		{0x00, 0x00, 0xFF}, /* Blue */
		{0xFF, 0x00, 0xFF}, /* Magenta */
	};

	int prng_colour = prev;
	while (prng_colour == prev)
		prng_colour = rand() % (sizeof(colours) / sizeof(struct colour));

	SDL_SetTextureColorMod(tex, colours[prng_colour].red, colours[prng_colour].green, colours[prng_colour].blue);
	prev = prng_colour;

}

SDL_Texture* loadTexture(char *path){

	SDL_Texture *newTexture = NULL;

	/* Load image */
	SDL_Surface *loadedSurface = IMG_Load(path);
	if (loadedSurface == NULL)
		fatalError("Couldn't load surface from image", IMG_GetError());
	/* Create texture from surface pixels */
	newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
	if (newTexture == NULL)
		fatalError("Couldn't create texture from image surface", SDL_GetError());
	/* Free old surface */
	SDL_FreeSurface(loadedSurface);
	return newTexture;

}

int main(){

	init();
	loadMedia();
	bool quit = false;
	SDL_Event e;
	struct {

		int x;
		int y;
		int x_direction;
		int y_direction;

	}flyState;


	flyState.x = rand() % (SCREEN_WIDTH - LOGO_WIDTH);
	flyState.y = rand() % (SCREEN_HEIGHT - LOGO_HEIGHT);
	flyState.x_direction = 1;
	flyState.y_direction = 1;

	SDL_SetTextureColorMod(logoTexture, 0x00, 0xFF, 0x00);

	while (! quit){

		/* Handle poll events on queue */
		while (SDL_PollEvent(&e) != 0){

			if (e.type == SDL_QUIT)
				quit = true;

		}

		/* Where the magic happens */

		/* Clear screen */
		SDL_RenderClear(gRenderer);

		/* Wall detection */

		if (flyState.x <= 0){
			flyState.x_direction = 1;
			changeColour(logoTexture);
		}
		else if (flyState.x >= (SCREEN_WIDTH - LOGO_WIDTH)){
			flyState.x_direction = -1;
			changeColour(logoTexture);
		}
		if (flyState.y <= 0){
			flyState.y_direction = 1;
			changeColour(logoTexture);
		}
		else if (flyState.y >= (SCREEN_HEIGHT - LOGO_HEIGHT)){
			flyState.y_direction = -1;
			changeColour(logoTexture);
		}

		flyState.x = flyState.x + flyState.x_direction;
		flyState.y = flyState.y + flyState.y_direction;

		SDL_Rect renderQuad = { flyState.x, flyState.y, LOGO_WIDTH, LOGO_HEIGHT };

		/* Render texture to screen */
		SDL_RenderCopy(gRenderer, logoTexture, NULL, &renderQuad);
		/* Update screen */
		SDL_RenderPresent(gRenderer);

		/* SDL_Delay(10); */

	}

	closeSDL(EXIT_SUCCESS);

	return 0;
}
