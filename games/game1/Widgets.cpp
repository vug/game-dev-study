#include "Widgets.h"

#include <gds.h>

//------------- Widget

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

//------------- Selector

Selector::Selector(const std::string& label, const std::vector<std::string> options) 
	: label(label), options(options) {}

void Selector::render(SDL_Renderer* renderer, TTF_Font* font) {
	renderText(label + ": " + options[selectionIx], {0xCC, 0x22, 0x33}, pos.x, pos.y, renderer, font, false);
}

void Selector::trigger() {
	selectionIx = positiveModulus(selectionIx + 1, options.size());
	callback();
}

const std::string& Selector::getSelection() const {
	return options[selectionIx];
}

void Selector::registerCallback(std::function<void()> func) {
	callback = func;
}


//------------- MenuPage

MenuPage::MenuPage(const SDL_Point& pos) : pos(pos) {}

Button& MenuPage::addButton(const std::string& text) {
	widgets.push_back(std::make_unique<Button>(text));

	Button& w = static_cast<Button&>(*widgets.back());
	w.setPosition(addPoints(pos, cursor));

	cursor = addPoints(cursor, {0, LINE_HEIGHT});
	return w;
}

Selector& MenuPage::addSelector(const std::string& label, const std::vector<std::string> options) {
	widgets.push_back(std::make_unique<Selector>(label, options));

	Selector& w = static_cast<Selector&>(*widgets.back());
	w.setPosition(addPoints(pos, cursor));

	cursor = addPoints(cursor, { 0, LINE_HEIGHT });
	return w;
}

void MenuPage::render(SDL_Renderer* renderer, TTF_Font* font) {
	for (auto& w : widgets)
		w->render(renderer, font);
	auto selectionIndicator = SDL_Rect{ pos.x - 2 * LINE_HEIGHT, pos.y + selectionIx * LINE_HEIGHT, LINE_HEIGHT, LINE_HEIGHT };

	SDL_SetRenderDrawColor(renderer, 0xCC, 0x22, 0x33, 0xFF);
	SDL_RenderFillRect(renderer, &selectionIndicator);
}

void MenuPage::handleKeys(SDL_Keycode key) {
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