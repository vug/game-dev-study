#pragma once

#include "Cell.h"
#include "Snake.h"

#include <SDL.h>
#include <SDL_ttf.h>

#include <random>

class MenuState;
class PlayingState;
class PauseState;

class StateManager {
public:
	MenuState* menuState;
	PlayingState* playingState;
	PauseState* pauseState;
	StateManager();
};


class State {
protected:
	StateManager* stateManager = nullptr;
public:
	State(StateManager* stateManager) : stateManager(stateManager) {}
	virtual void handleEvent(const SDL_Event& e) = 0;
	virtual State* update(uint32_t deltaTime) = 0;
	virtual void render(SDL_Renderer* gRenderer, TTF_Font* gFont) = 0;
	virtual ~State() = default;
};

class MenuState : public State {
private:
	SDL_Keycode lastKey{};

public:
	MenuState(StateManager* stateManager);
	void handleEvent(const SDL_Event& e) final;
	State* update(uint32_t deltaTime) final;
	void render(SDL_Renderer* gRenderer, TTF_Font* gFont) final;
};

class PlayingState : public State {
private:
	int32_t gridSize{ 15 };
	SDL_Keycode lastKey;
	Snake snake;
	Cell apple;
	uint32_t score{};
	uint32_t timer{};
	std::mt19937 rnd = std::mt19937{ std::random_device{}() };
	//State* state;
public:
	int period = 200;

public:
	PlayingState(StateManager* stateManager);

	void handleEvent(const SDL_Event& e)  final;

	void placeApple();

	void render(SDL_Renderer* gRenderer, TTF_Font* gFont) final;

	State* update(uint32_t deltaTime) final;
};

class PauseState : public State {
private:
	SDL_Keycode lastKey{};

public:
	PauseState(StateManager* stateManager);

	void handleEvent(const SDL_Event& e) final;

	State* update(uint32_t deltaTime) final;

	void render(SDL_Renderer* gRenderer, TTF_Font* gFont) final;
};
