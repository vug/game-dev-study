#include <SDL.h>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <vector>

const int SIZE = 800;

int positiveModulus(int num, int mod)
{
	int result = num % mod;
	if (result < 0)
		result += mod;
	return result;
}

enum class Direction {
	UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3
};

class Cell {
public:
	int x{};
	int y{};

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
		default:
			assert(false); // unknown direction
		}
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
	// from head to tail
	std::vector<Cell> cells;
	Direction dir;
public:
	Snake() // TODO: take gridSize as argument to place it at the center
		: cells{ Cell{5, 5}, Cell{6, 5}, Cell{7, 5}, Cell{8,5}, Cell{9,5} }, dir{ Direction::LEFT } {}
	const std::vector<Cell>& getCells() const { return cells; }
	// signs are opposite because the rendering surface is upside-down
	void turnRight() { dir = static_cast<Direction>(positiveModulus(static_cast<int>(dir) - 1, 4)); }
	void turnLeft() { dir = static_cast<Direction>(positiveModulus(static_cast<int>(dir) + 1, 4)); }
	void move() {
		for (size_t ix = cells.size() - 1; ix > 0; --ix)
			cells[ix] = cells[ix - 1];
		const Cell dc = Cell::deltaCell(dir);
		cells[0] = cells[0].addCell(dc);
	}
	bool hasCell(const Cell& other) {
		bool result = false;
		for (const Cell& cell : cells)
			if (cell.isSameAs(other))
				result = true;
		return true;
	}
	bool hasBitItself() {
		const Cell& head = cells[0];
		for (size_t ix = 1; ix < cells.size(); ++ix) {
			const Cell& other = cells[ix];
			if (head.isSameAs(other))
				return true;
		}
		return false;
	}
};

class Game {
private:
	uint32_t gridSize = 10;
	SDL_Keycode lastKey;
	Snake snake;
	Cell apple;
	std::mt19937 rnd = std::mt19937{ std::random_device{}() };

public:
	Game() : lastKey{ SDLK_UNKNOWN } {
		placeApple();
	}

	void keyPressed(SDL_Keycode key) {
		lastKey = key;
	}

	void placeApple() {
		std::vector<Cell> availableCells;
		std::vector<Cell> out; // a vector, not a single cell because of std::sample API
		const int numCells = gridSize * gridSize;
		for (int i = 0; i < numCells; ++i) {
			Cell cell{ i % gridSize , i / gridSize};
			if (snake.hasCell(cell))
				availableCells.push_back(cell);
		}
		std::sample(availableCells.begin(), availableCells.end(), std::back_inserter(out), 1, rnd);
		apple = out[0];
	}

	void render(SDL_Renderer* gRenderer) {
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

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
	}

	void tick() {
		switch (lastKey) {
		case SDLK_LEFT:
			//std::cout << "Left\n";
			snake.turnLeft();
			break;
		case SDLK_RIGHT:
			//std::cout << "Right\n";
			snake.turnRight();
			break;
		}
		lastKey = SDLK_UNKNOWN;

		snake.move();
		const Cell& head = snake.getCells()[0];
		if (head.isAtGridWalls(gridSize))
			std::cout << "GAME OVER: snake hit wall\n";
		else if (snake.hasBitItself())
			std::cout << "GAME OVER: snake bit itself\n";
		else if (snake.getCells()[0].isSameAs(apple)) {
			// 
			placeApple();
		}
	}
};

int main(int argc, char* args[]) {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SIZE, SIZE, SDL_WINDOW_SHOWN);
	SDL_Surface* gScreenSurface = SDL_GetWindowSurface(gWindow);
	SDL_Renderer* gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);

	SDL_UpdateWindowSurface(gWindow);
	SDL_Event e;
	bool quit = false;
	Game game;

	uint32_t time = SDL_GetTicks();
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT)
				quit = true;
			else if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
				game.keyPressed(e.key.keysym.sym);
			}
		}

		if (SDL_GetTicks() - time > 200) {
			game.tick();
			time = SDL_GetTicks();
		}

		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		game.render(gRenderer);

		SDL_RenderPresent(gRenderer);
	}

	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	SDL_Quit();
	return 0;
}