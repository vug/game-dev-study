#include "Widgets.h"

#include <gds.h>

//------------- Button

Button::Button(const std::string& text) : text(text) {}
void Button::render(SDL_Renderer* renderer, TTF_Font* font) {
	renderText(text, { 0xCC, 0x22, 0x33 }, x, y, renderer, font, false);
}

void Button::trigger() {
	callback();
}

void Button::registerCallback(std::function<void()> func) {
	callback = func;
}

Button& Menu::addButton(const std::string& text) {
	widgets.push_back(std::make_unique<Button>(text));
	return static_cast<Button&>(*widgets.back());
}

void Menu::render(SDL_Renderer* renderer, TTF_Font* font) {
	for (auto& w : widgets)
		w->render(renderer, font);
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