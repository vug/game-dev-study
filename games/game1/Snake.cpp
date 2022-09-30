#include "Snake.h"

#include <gds.h>

Snake::Snake(Cell head, uint32_t length, Direction dir)
	: dir{ dir } {
	Cell cell = head;
	for (int i = 0; i < length; ++i) {
		cells.push_back(cell);
		cell = cell.addCell(Cell::deltaCell(dir));
	}
}

// signs are opposite because the rendering surface is upside-down
void Snake::turnRight() { dir = static_cast<Direction>(positiveModulus(static_cast<int>(dir) - 1, 4)); }
void Snake::turnLeft() { dir = static_cast<Direction>(positiveModulus(static_cast<int>(dir) + 1, 4)); }

Cell Snake::getNextCell() {
	return getHead().addCell(Cell::deltaCell(dir));
}

void Snake::move() {
	for (size_t ix = cells.size() - 1; ix > 0; --ix)
		cells[ix] = cells[ix - 1];
	const Cell dc = Cell::deltaCell(dir);
	cells[0] = cells[0].addCell(dc);
}

void Snake::elongate() {
	cells.insert(cells.begin(), getNextCell());
}

bool Snake::hasCell(const Cell& other) {
	for (const Cell& cell : cells)
		if (cell.isSameAs(other))
			return true;
	return false;
}

bool Snake::willBiteItself(const Cell& nextCell) {
	return hasCell(nextCell) && !nextCell.isSameAs(getTail());
}