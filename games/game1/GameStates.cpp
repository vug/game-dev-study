#include "GameStates.h"

#include <algorithm>
#include <iostream>
#include <random>
#include <string>

const int SIZE = 800;

void renderText(const std::string& text, SDL_Color color, int x, int y, SDL_Renderer* renderer, TTF_Font* font) {
	SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), color);   // anti-aliased glyphs, TTF_RenderText_Solid() for aliased glyphs
	if (textSurface == nullptr)
		std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << "\n";
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);

	int texW, texH;
	SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
	SDL_Rect dstrect = { x, y, texW, texH };
	SDL_RenderCopy(renderer, texture, NULL, &dstrect);

	SDL_DestroyTexture(texture);
	SDL_FreeSurface(textSurface);
}

//------------- MenuState

MenuState::MenuState(StateManager* stateManager) : State(stateManager) {}

void MenuState::handleEvent(const SDL_Event& e) {
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
		lastKey = e.key.keysym.sym;
}

State* MenuState::update(uint32_t deltaTime) {
	State* result = this;
	if (lastKey == SDLK_SPACE)
		result = stateManager->playingState;
	lastKey = SDLK_UNKNOWN;

	return result;
}

void MenuState::render(SDL_Renderer* gRenderer, TTF_Font* gFont) {
	SDL_SetRenderDrawColor(gRenderer, 0x88, 0x88, 0x88, 0xFF);
	SDL_RenderClear(gRenderer);

	renderText("Hungry Snake", { 0xCC, 0x22, 0x33 }, SIZE / 2, SIZE / 2, gRenderer, gFont);
}


//------------- PlayingState


PlayingState::PlayingState(StateManager* stateManager) : State(stateManager), lastKey{ SDLK_UNKNOWN }, snake{ Cell{gridSize / 2, gridSize / 2}, 2, Direction::LEFT } {
	for (const Cell& cell : snake.getCells())
		assert(!cell.isAtGridWalls(gridSize));
	placeApple();
}

void PlayingState::handleEvent(const SDL_Event& e) {
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
		lastKey = e.key.keysym.sym;
}

void PlayingState::placeApple() {
	std::vector<Cell> availableCells{};
	std::vector<Cell> out; // a vector, not a single cell because of std::sample API
	const int32_t numCells = gridSize * gridSize;
	for (int32_t i = 0; i < numCells; ++i) {
		Cell cell{ i % gridSize , i / gridSize };
		if (!snake.hasCell(cell))
			availableCells.push_back(cell);
	}
	if (availableCells.size() == 0) {
		std::cout << "CONGRATULATIONS: Longest snake!\n";
		return;
	}
	std::sample(availableCells.begin(), availableCells.end(), std::back_inserter(out), 1, rnd);
	apple = out[0];
}

void PlayingState::render(SDL_Renderer* gRenderer, TTF_Font* gFont) {
	// Clear
	SDL_SetRenderDrawColor(gRenderer, 0x88, 0x88, 0x88, 0xFF);
	SDL_RenderClear(gRenderer);

	// Render Game Area
	const float rectSide = static_cast<float>(SIZE) / gridSize;

	{
		SDL_SetRenderDrawColor(gRenderer, 0xAA, 0x00, 0x00, 0xFF);
		const SDL_FRect rect = { apple.x * rectSide, apple.y * rectSide, rectSide, rectSide };
		SDL_RenderFillRectF(gRenderer, &rect);
	}

	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
	for (const Cell& cell : snake.getCells()) {
		const SDL_FRect rect = { cell.x * rectSide, cell.y * rectSide, rectSide, rectSide };
		SDL_RenderFillRectF(gRenderer, &rect);
	}

	// Render texts such as score
	{
		std::string text = "score: " + std::to_string(score);
		renderText(text, { 0xCC, 0xCC, 0xCC }, 0, 0, gRenderer, gFont);
	}
}

State* PlayingState::update(uint32_t deltaTime) {
	State* result = this;

	timer += deltaTime;
	if (timer < period)
		return result;
	timer -= period;

	switch (lastKey) {
	case SDLK_LEFT:
		snake.turnLeft();
		break;
	case SDLK_RIGHT:
		snake.turnRight();
		break;
	case SDLK_p:
		result = stateManager->pauseState;
		break;
	case SDLK_UNKNOWN:
		break;
	}
	lastKey = SDLK_UNKNOWN;

	const Cell& nextCell = snake.getNextCell();
	if (nextCell.isAtGridWalls(gridSize))
		std::cout << "GAME OVER: snake hit wall\n";
	else if (snake.willBiteItself(nextCell))
		std::cout << "GAME OVER: snake bit itself\n";
	else if (nextCell.isSameAs(apple)) {
		snake.elongate();
		placeApple();
		++score;
	}
	else
		snake.move();

	return result;
}


//------------- PauseState


PauseState::PauseState(StateManager* stateManager) : State(stateManager) {}

void PauseState::handleEvent(const SDL_Event& e) {
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
		lastKey = e.key.keysym.sym;
}

State* PauseState::update(uint32_t deltaTime) {
	State* result = this;
	if (lastKey == SDLK_p)
		result = stateManager->playingState;
	lastKey = SDLK_UNKNOWN;

	return result;
}

void PauseState::render(SDL_Renderer* gRenderer, TTF_Font* gFont) {
	SDL_SetRenderDrawColor(gRenderer, 0x88, 0x88, 0x88, 0xFF);
	SDL_RenderClear(gRenderer);

	renderText("Paused...", { 0xCC, 0x22, 0x33 }, SIZE / 2, SIZE / 2, gRenderer, gFont);
}

//------------- StateManager

StateManager::StateManager()
	: menuState(new MenuState{ this }), playingState(new PlayingState{ this }), pauseState(new PauseState{ this }) {}
