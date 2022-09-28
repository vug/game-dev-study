#include <SDL.h>
#include <SDL_ttf.h>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <string>
#include <vector>

const int SIZE = 800;

int positiveModulus(int num, int mod)
{
	int result = num % mod;
	if (result < 0)
		result += mod;
	return result;
}

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

enum class Direction {
	UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3
};

class Cell {
public:
	int32_t x{};
	int32_t y{};

public:
	static Cell deltaCell(Direction dir) {
		switch (dir) {
		case Direction::UP:
			return Cell{ 0, 1 };
		case Direction::RIGHT:
			return Cell{ 1, 0 };
		case Direction::DOWN:
			return Cell{ 0, -1 };
		case Direction::LEFT:
			return Cell{ -1, 0 };
		}
		assert(false); // unknown direction
	}

	Cell addCell(const Cell& other) const {
		return Cell{ x + other.x, y + other.y };
	}

	bool isSameAs(const Cell& other) const {
		return x == other.x && y == other.y;
	}

	bool isAtGridWalls(int gridSize) const {
		return x == -1 || x == gridSize || y == -1 || y == gridSize;
	}
};



class Snake {
private:
	// from head to tail
	std::vector<Cell> cells;
	Direction dir;

public:
	Snake(Cell head, uint32_t length, Direction dir)
		: dir{ dir } {
		Cell cell = head;
		for (int i = 0; i < length; ++i) {
			cells.push_back(cell);
			cell = cell.addCell(Cell::deltaCell(dir));
		}
	}

	const std::vector<Cell>& getCells() const { return cells; }

	const Cell& getHead() const { return cells[0]; }
	const Cell& getTail() const { return cells.back(); }

	// signs are opposite because the rendering surface is upside-down
	void turnRight() { dir = static_cast<Direction>(positiveModulus(static_cast<int>(dir) - 1, 4)); }
	void turnLeft() { dir = static_cast<Direction>(positiveModulus(static_cast<int>(dir) + 1, 4)); }

	Cell getNextCell() {
		return getHead().addCell(Cell::deltaCell(dir));
	}

	void move() {
		for (size_t ix = cells.size() - 1; ix > 0; --ix)
			cells[ix] = cells[ix - 1];
		const Cell dc = Cell::deltaCell(dir);
		cells[0] = cells[0].addCell(dc);
	}

	void elongate() {
		cells.insert(cells.begin(), getNextCell());
	}

	bool hasCell(const Cell& other) {
		for (const Cell& cell : cells)
			if (cell.isSameAs(other))
				return true;
		return false;
	}

	bool willBiteItself(const Cell& nextCell) {
		return hasCell(nextCell) && !nextCell.isSameAs(getTail());
	}
};

enum class StateID
{
	Menu, Playing, Pause
};

class StateManager;

class State {
protected:
	StateManager* stateManager = nullptr;
public:
	State(StateManager* stateManager) : stateManager(stateManager) {}
	virtual void handleEvent(const SDL_Event& e) = 0;
	virtual StateID update(uint32_t deltaTime) = 0;
	virtual void render(SDL_Renderer* gRenderer, TTF_Font* gFont) = 0;
	virtual ~State() = default;
};

class MenuState : public State {
private:
	SDL_Keycode lastKey{};

public:
	MenuState(StateManager* stateManager) : State(stateManager) {}
	void handleEvent(const SDL_Event& e) final {
		if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
			lastKey = e.key.keysym.sym;
	}

	StateID update(uint32_t deltaTime) final {
		StateID result = StateID::Menu;
		if (lastKey == SDLK_SPACE)
			result = StateID::Playing;
		lastKey = SDLK_UNKNOWN;

		return result;
	}

	void render(SDL_Renderer* gRenderer, TTF_Font* gFont) final {
		SDL_SetRenderDrawColor(gRenderer, 0x88, 0x88, 0x88, 0xFF);
		SDL_RenderClear(gRenderer);

		renderText("Hungry Snake", { 0xCC, 0x22, 0x33 }, SIZE / 2, SIZE / 2, gRenderer, gFont);
	}
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
	PlayingState(StateManager* stateManager) : State(stateManager), lastKey{ SDLK_UNKNOWN }, snake{ Cell{gridSize / 2, gridSize / 2}, 2, Direction::LEFT } {
		for (const Cell& cell : snake.getCells())
			assert(!cell.isAtGridWalls(gridSize));
		placeApple();
	}

	void handleEvent(const SDL_Event& e)  final {
		if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
			lastKey = e.key.keysym.sym;
	}

	void placeApple() {
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

	void render(SDL_Renderer* gRenderer, TTF_Font* gFont) final {
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

	StateID update(uint32_t deltaTime) final {
		StateID result = StateID::Playing;

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
			result = StateID::Pause;
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
};

class PauseState : public State {
private:
	SDL_Keycode lastKey{};

public:
	PauseState(StateManager* stateManager) : State(stateManager) {}

	void handleEvent(const SDL_Event& e) final {
		if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
			lastKey = e.key.keysym.sym;
	}

	StateID update(uint32_t deltaTime) final {
		StateID result = StateID::Pause;
		if (lastKey == SDLK_p)
			result = StateID::Playing;
		lastKey = SDLK_UNKNOWN;

		return result;
	}

	void render(SDL_Renderer* gRenderer, TTF_Font* gFont) final {
		SDL_SetRenderDrawColor(gRenderer, 0x88, 0x88, 0x88, 0xFF);
		SDL_RenderClear(gRenderer);

		renderText("Paused...", { 0xCC, 0x22, 0x33 }, SIZE / 2, SIZE / 2, gRenderer, gFont);
	}
};

class StateManager {
public:
	MenuState menuState;
	PlayingState playingState;
	PauseState pauseState;
public:
	StateManager() : menuState(this), playingState(this), pauseState(this) {}
};


class Game {
private:
	StateManager stateManager;
	State* state = &stateManager.menuState;
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
			StateID nextStateID = state->update(deltaTime);
			switch (nextStateID) {
			case StateID::Menu:
				state = &stateManager.menuState;
				break;
			case StateID::Playing:
				state = &stateManager.playingState;
				break;
			case StateID::Pause:
				state = &stateManager.pauseState;
				break;
			default:
				assert(false); // unknown state
			}

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