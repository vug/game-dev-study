#include "Widgets.h"

// TODO: Fill out implementation
Button::Button(const std::string& text) : text(text) {}
void Button::render(SDL_Renderer* renderer, TTF_Font* font) {}
void Button::trigger() {}
void Button::registerCallback(std::function<void()> func) {}

Button& Menu::addButton(const std::string& text) {
	widgets.push_back(std::make_unique<Button>(text));
	return static_cast<Button&>(*widgets.back());
}

void Menu::render(SDL_Renderer* renderer, TTF_Font* font) {}
void Menu::handleKeys(SDL_Keycode key) {}