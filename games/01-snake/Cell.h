#pragma once

#include <cassert>
#include <cstdint>

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
