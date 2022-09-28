#pragma once

#include "Cell.h"

#include <vector>

int positiveModulus(int num, int mod);

class Snake {
private:
	// from head to tail
	std::vector<Cell> cells;
	Direction dir;

public:
	Snake(Cell head, uint32_t length, Direction dir);

	inline const std::vector<Cell>& getCells() const { return cells; }
	inline const Cell& getHead() const { return cells[0]; }
	inline const Cell& getTail() const { return cells.back(); }

	// signs are opposite because the rendering surface is upside-down
	void turnRight();
	void turnLeft();

	Cell getNextCell();

	void move();

	void elongate();

	bool hasCell(const Cell& other);

	bool willBiteItself(const Cell& nextCell);
};
