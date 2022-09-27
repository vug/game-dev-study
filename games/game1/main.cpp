#include <SDL.h>

#include <iostream>

int main(int argc, char* args[]) {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 800, SDL_WINDOW_SHOWN);
	SDL_Surface* gScreenSurface = SDL_GetWindowSurface(gWindow);
	SDL_Renderer* gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
	std::cout << "Hello, SDL!\n";

	SDL_UpdateWindowSurface(gWindow);
	SDL_Event e; 
	bool quit = false; 
	while (!quit) { 
		while (SDL_PollEvent(&e)) { 
			if (e.type == SDL_QUIT) quit = true; 
		}

		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		SDL_Rect fillRect = { 800 / 4, 800 / 4, 800 / 2, 800 / 2 };
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
		SDL_RenderFillRect(gRenderer, &fillRect);

		SDL_RenderPresent(gRenderer);
	}

	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	SDL_Quit();
	return 0;
}