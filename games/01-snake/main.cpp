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

gds::Sdl gds::sdl = gds::Sdl("Snake", SIZE, SIZE);

class Game {
private:
	StateManager stateManager;
	State* state = stateManager.menuState.get();
public:
	void run() {
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

			SDL_SetRenderDrawColor(gds::sdl.renderer, 0xFF, 0x00, 0xFF, 0xFF);
			SDL_RenderClear(gds::sdl.renderer);

			state->render();

			gds::sdl.renderPresent();
		}
	}
};

int main(int argc, char* args[]) {
	gds::sdl.loadFont(gds::DEFAULT_FONT, "assets/fonts/enter_command/EnterCommand.ttf", 28); // "c:\\Windows\\Fonts\\vgaoem.fon"; // arial.ttf"
	gds::sdl.loadFont(gds::TITLE_FONT, "assets/fonts/enter_command/EnterCommand.ttf", 40);

	Game game;
	game.run();
	return 0;
}