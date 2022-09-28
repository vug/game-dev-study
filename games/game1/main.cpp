#include "GameStates.h"
#include "Snake.h"

#include <SDL.h>
#include <SDL_ttf.h>

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

const int SIZE = 800;

class Game {
private:
	StateManager stateManager;
	State* state = stateManager.menuState;
public:
	void run() {
		SDL_Init(SDL_INIT_VIDEO);
		TTF_Init();
		SDL_Window* gWindow = SDL_CreateWindow("Snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SIZE, SIZE, SDL_WINDOW_SHOWN);
		//SDL_Surface* gScreenSurface = SDL_GetWindowSurface(gWindow);
		SDL_Renderer* gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
		const char* fontFile = "assets/fonts/enter_command/EnterCommand.ttf"; // "c:\\Windows\\Fonts\\vgaoem.fon"; // arial.ttf"
		TTF_Font* gFont = TTF_OpenFont(fontFile, 28);
		if (gFont == nullptr)
			std::cout << "font not found! " << TTF_GetError() << "\n";
		TTF_SetFontStyle(gFont, TTF_STYLE_NORMAL);

		SDL_UpdateWindowSurface(gWindow);
		SDL_Event e;
		bool quit = false;

		uint32_t time = SDL_GetTicks();
		while (!quit) {
			while (SDL_PollEvent(&e)) {
				if (e.type == SDL_QUIT)
					quit = true;
				else
					state->handleEvent(e);
			}

			uint32_t deltaTime = SDL_GetTicks() - time;
			time = SDL_GetTicks();
			// State Manager
			state = state->update(deltaTime);

			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0xFF, 0xFF);
			SDL_RenderClear(gRenderer);

			state->render(gRenderer, gFont);

			SDL_RenderPresent(gRenderer);
		}

		TTF_CloseFont(gFont);
		TTF_Quit();
		SDL_DestroyRenderer(gRenderer);
		SDL_DestroyWindow(gWindow);
		SDL_Quit();
	}
};

int main(int argc, char* args[]) {
	Game game;
	game.run();
	return 0;
}