#include "GameStates.h"

#include "Widgets.h"

#include "gds.h"

#include <algorithm>
#include <iostream>
#include <random>
#include <string>

const int SIZE = 800;

//------------- MenuState

MenuState::MenuState(StateManager& stateManager) 
	: State(stateManager), mainPage({SIZE / 2, SIZE / 2}), settingsPage({ SIZE / 2, SIZE / 2 }), menu(mainPage) {

	// Main Page
	{
		Button& startButton = mainPage.addButton("Start");
		auto callback = [&]() { 
			stateManager.playingState->restart();
			this->nextState = stateManager.playingState.get();
		};
		startButton.registerCallback(callback);
	}

	{
		Button& settingsButton = mainPage.addButton("Settings");
		auto callback = [&]() {
			menu.pushPage(settingsPage);
		};
		settingsButton.registerCallback(callback);
	}

	{
		Button& exitButton = mainPage.addButton("Exit");
		auto callback = [&]() {
			SDL_Event exit{};
			exit.type = SDL_QUIT;
			exit.quit = SDL_QuitEvent{ SDL_QUIT, SDL_GetTicks() };
			SDL_PushEvent(&exit);
		};
		exitButton.registerCallback(callback);
	}

	// Settings Page
	{
		// Grid Size
		static const std::string SMALL = "Small";
		static const std::string MEDIUM = "Medium";
		static const std::string LARGE = "Large";
		Selector& sizeSelector = settingsPage.addSelector("Area Size", { SMALL, MEDIUM, LARGE });
		auto callback = [&]() {
			int32_t& size = stateManager.playingState->gridSize;
			const std::string& selected = sizeSelector.getSelection();
			if (selected == SMALL)
				size = 10;
			else if (selected == MEDIUM)
				size = 20;
			else if (selected == LARGE)
				size = 40;
		};
		sizeSelector.registerCallback(callback);
	}

	{
		// Speed
		static const std::string SLOW = "Slow";
		static const std::string MEDIUM = "Medium";
		static const std::string FAST = "Fast";
		Selector& speedSelector = settingsPage.addSelector("Speed", { SLOW, MEDIUM, FAST });
		auto callback = [&]() {
			int32_t& period = stateManager.playingState->period;
			const std::string& selected = speedSelector.getSelection();
			if (selected == SLOW)
				period = 400;
			else if (selected == MEDIUM)
				period = 200;
			else if (selected == FAST)
				period = 100;
		};
		speedSelector.registerCallback(callback);
	}

	{
		Button& backButton = settingsPage.addButton("Back");
		auto callback = [&]() {
			menu.popPage();
		};
		backButton.registerCallback(callback);
	}

	menu.pushPage(mainPage);
}

void MenuState::handleEvent(const SDL_Event& e) {
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
		lastKey = e.key.keysym.sym;
}

State* MenuState::update(uint32_t deltaTime) {
	State* result = this;
	// TODO: return switchToNextStateIfAny();
	if (nextState) {
		result = nextState;
		nextState = nullptr;
		return result;
	}
		
	menu.handleKeys(lastKey);
	lastKey = SDLK_UNKNOWN;

	return result;
}

void MenuState::render(SDL_Renderer* gRenderer, TTF_Font* gFont) {
	SDL_SetRenderDrawColor(gRenderer, 0x88, 0x88, 0x88, 0xFF);
	SDL_RenderClear(gRenderer);

	menu.render(gRenderer, gFont);

	const int leftMargin = 15;
	const int lineHeight = 25;
	gds::renderText("Hungry Snake", { 0xCC, 0x22, 0x33 }, SIZE / 2, 200, gFont, true);
	gds::renderText("LEFT ARROW turns the snake to the left", { 0xCC, 0x22, 0x33 }, leftMargin, SIZE - lineHeight * 3, gFont);
	gds::renderText("RIGHT ARROW turns the snake to the right", { 0xCC, 0x22, 0x33 }, leftMargin, SIZE - lineHeight * 2, gFont);
	gds::renderText("P pauses the game", { 0xCC, 0x22, 0x33 }, leftMargin, SIZE - lineHeight * 1, gFont);
}


//------------- PlayingState


PlayingState::PlayingState(StateManager& stateManager) : State(stateManager), lastKey{ SDLK_UNKNOWN }, snake{ Cell{gridSize / 2, gridSize / 2}, 2, Direction::LEFT } {
	for (const Cell& cell : snake.getCells())
		assert(!cell.isAtGridWalls(gridSize));
	placeApple();
}

void PlayingState::handleEvent(const SDL_Event& e) {
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
		lastKey = e.key.keysym.sym;
}

void PlayingState::placeApple() {
	const int32_t numCells = gridSize * gridSize;
	std::vector<Cell> availableCells{};
	availableCells.reserve(numCells);
	std::vector<Cell> out; // a vector, not a single cell because of std::sample API
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

void PlayingState::restart() {
	score = 0;
	snake = Snake{ Cell{gridSize / 2, gridSize / 2}, 2, Direction::LEFT };
	placeApple();
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
		gds::renderText(text, { 0xCC, 0xCC, 0xCC }, 0, 0, gFont);
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
		result = stateManager.pauseState.get();
		break;
	case SDLK_UNKNOWN:
		break;
	}
	lastKey = SDLK_UNKNOWN;

	const Cell& nextCell = snake.getNextCell();
	if (nextCell.isAtGridWalls(gridSize)) {
		GameOverState& gameOver = *stateManager.gameOverState;
		result = &gameOver;
		gameOver.setGameOverReason("(Snake hit the wall.)");
	}
	else if (snake.willBiteItself(nextCell)) {
		GameOverState& gameOver = *stateManager.gameOverState;
		result = &gameOver;
		gameOver.setGameOverReason("(Snake bit itself.)");
	}
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

PauseState::PauseState(StateManager& stateManager) : State(stateManager) {}

void PauseState::handleEvent(const SDL_Event& e) {
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
		lastKey = e.key.keysym.sym;
}

State* PauseState::update(uint32_t deltaTime) {
	State* result = this;
	if (lastKey == SDLK_p)
		result = stateManager.playingState.get();
	lastKey = SDLK_UNKNOWN;

	return result;
}

void PauseState::render(SDL_Renderer* gRenderer, TTF_Font* gFont) {
	// Render game area without evolving it
	stateManager.playingState->render(gRenderer, gFont);

	// Draw a semi-transparent fullscreen quad overlay
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x55);
	SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
	const SDL_Rect rect = { 0, 0, SIZE, SIZE};
	SDL_RenderFillRect(gRenderer, &rect);
	SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_NONE);

	// Pause rendering specific draw calls
	gds::renderText("Paused...", { 0xCC, 0x22, 0x33 }, SIZE / 2, SIZE / 2, gFont, true);
}

//------------- GameOverState

GameOverState::GameOverState(StateManager& stateManager) : State(stateManager) {}

void GameOverState::setGameOverReason(const std::string& text) {
	gameOverReason = text;
}

void GameOverState::handleEvent(const SDL_Event& e) {
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
		lastKey = e.key.keysym.sym;
}

State* GameOverState::update(uint32_t deltaTime) {
	State* result = this;
	if (lastKey == SDLK_SPACE) {
		result = stateManager.menuState.get();
		stateManager.playingState->restart();
	}
	lastKey = SDLK_UNKNOWN;

	return result;
}

void GameOverState::render(SDL_Renderer* gRenderer, TTF_Font* gFont) {
	// Render game area without evolving it
	stateManager.playingState->render(gRenderer, gFont);

	// Draw a semi-transparent fullscreen quad overlay
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0x99);
	SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
	const SDL_Rect rect = { 0, 0, SIZE, SIZE };
	SDL_RenderFillRect(gRenderer, &rect);
	SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_NONE);

	// GameOver rendering specific draw calls
	gds::renderText("Game Over", { 0xCC, 0x22, 0x33 }, SIZE / 2, SIZE / 2, gFont, true);
	gds::renderText(gameOverReason.c_str(), {0xCC, 0x22, 0x33}, SIZE / 2, SIZE / 2 + 30, gFont, true);
	gds::renderText("Press SPACE to return to main menu", { 0xCC, 0x22, 0x33 }, SIZE / 2, SIZE - 30, gFont, true);

}

//------------- StateManager

StateManager::StateManager() : 
	menuState(std::make_unique<MenuState>(*this)), 
	playingState(std::make_unique<PlayingState>(*this)), 
	pauseState(std::make_unique<PauseState>(*this)),
	gameOverState(std::make_unique<GameOverState>(*this))
	{}
