#include "Widgets.h"

#include <gds.h>

//------------- Button

void Widget::setPosition(const SDL_Point& p) {
	pos = p; 
}


//------------- Button

Button::Button(const std::string& text) : text(text) {}
void Button::render(SDL_Renderer* renderer, TTF_Font* font) {
	renderText(text, { 0xCC, 0x22, 0x33 }, pos.x, pos.y, renderer, font, false);
}

void Button::trigger() {
	callback();
}

void Button::registerCallback(std::function<void()> func) {
	callback = func;
}


//------------- Menu

Menu::Menu(const SDL_Point& pos) : pos(pos) {}

Button& Menu::addButton(const std::string& text) {
	widgets.push_back(std::make_unique<Button>(text));

	Button& b = static_cast<Button&>(*widgets.back());
	b.setPosition(addPoints(pos, cursor));

	cursor = addPoints(cursor, {0, LINE_HEIGHT});
	return b;
}

void Menu::render(SDL_Renderer* renderer, TTF_Font* font) {
	for (auto& w : widgets)
		w->render(renderer, font);
	auto selectionIndicator = SDL_Rect{ pos.x - 2 * LINE_HEIGHT, pos.y + selectionIx * LINE_HEIGHT, LINE_HEIGHT, LINE_HEIGHT };

	SDL_SetRenderDrawColor(renderer, 0xCC, 0x22, 0x33, 0xFF);
	SDL_RenderFillRect(renderer, &selectionIndicator);
}

void Menu::handleKeys(SDL_Keycode key) {
	switch (key) {
	case SDLK_UP:
		selectionIx = positiveModulus(selectionIx - 1, widgets.size());
		break;
	case SDLK_DOWN:
		selectionIx = (selectionIx + 1) % widgets.size();
		break;
	case SDLK_RETURN:
		widgets[selectionIx]->trigger();
		break;
	default:
		break;
	}
}