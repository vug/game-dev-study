#pragma once

#include "Cell.h"
#include "Snake.h"
#include "Widgets.h"

#include <SDL.h>
#include <SDL_ttf.h>

#include <memory>
#include <random>

class State;
class MenuState;
class PlayingState;
class PauseState;
class GameOverState;

class StateManager {
public:
	std::unique_ptr<MenuState> menuState;
	std::unique_ptr<PlayingState> playingState;
	std::unique_ptr<PauseState> pauseState;
	std::unique_ptr<GameOverState> gameOverState;
	StateManager();
};


class State {
protected:
	StateManager& stateManager;
public:
	State(StateManager& stateManager) : stateManager(stateManager) {}
	virtual void handleEvent(const SDL_Event& e) = 0;
	virtual State* update(uint32_t deltaTime) = 0;
	virtual void render() = 0;
	virtual ~State() = default;
};

class MenuState : public State {
private:
	SDL_Keycode lastKey{};
	MenuPage mainPage;
	MenuPage settingsPage;
	Menu menu;
	State* nextState = nullptr;

public:
	MenuState(StateManager& stateManager);
	void handleEvent(const SDL_Event& e) final;
	State* update(uint32_t deltaTime) final;
	void render() final;
};

class PlayingState : public State {
private:
	SDL_Keycode lastKey;
	Snake snake;
	Cell apple;
	uint32_t score{};
	uint32_t timer{};
	std::mt19937 rnd = std::mt19937{ std::random_device{}() };
	//State* state;
public:
	int32_t gridSize{ 15 };
	int32_t period = 200;

public:
	PlayingState(StateManager& stateManager);

	void restart();

	void placeApple();

	void handleEvent(const SDL_Event& e)  final;

	void render() final;

	State* update(uint32_t deltaTime) final;
};

class PauseState : public State {
private:
	SDL_Keycode lastKey{};

public:
	PauseState(StateManager& stateManager);

	void handleEvent(const SDL_Event& e) final;

	State* update(uint32_t deltaTime) final;

	void render() final;
};

class GameOverState : public State {
private:
	SDL_Keycode lastKey{};
	std::string gameOverReason = "NO REASON GIVEN";

public:
	GameOverState(StateManager& stateManager);

	void setGameOverReason(const std::string& text);

	void handleEvent(const SDL_Event& e) final;

	State* update(uint32_t deltaTime) final;

	void render() final;
};
