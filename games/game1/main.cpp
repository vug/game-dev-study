#include "GameStates.h"
#include "Snake.h"

#include <gds.h>

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
	State* state = stateManager.menuState.get();
public:
	void run() {
		gds::Sdl sdl = gds::Sdl("Snake", SIZE, SIZE);

		const char* fontFile = "assets/fonts/enter_command/EnterCommand.ttf"; // "c:\\Windows\\Fonts\\vgaoem.fon"; // arial.ttf"
		TTF_Font* gFont = TTF_OpenFont(fontFile, 28);
		if (gFont == nullptr)
			std::cout << "font not found! " << TTF_GetError() << "\n";
		TTF_SetFontStyle(gFont, TTF_STYLE_NORMAL);

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

			SDL_SetRenderDrawColor(sdl.renderer, 0xFF, 0x00, 0xFF, 0xFF);
			SDL_RenderClear(sdl.renderer);

			state->render(sdl.renderer, gFont);

			sdl.renderPresent();
		}

		TTF_CloseFont(gFont);
	}
};

int main(int argc, char* args[]) {
	Game game;
	game.run();
	return 0;
}