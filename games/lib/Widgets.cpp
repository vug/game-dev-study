#include "Widgets.h"

#include <gds.h>

#include <cassert>

namespace gds {

//------------- Widget

void Widget::setPosition(const SDL_Point& p) {
	pos = p; 
}


//------------- Button

Button::Button(const std::string& text) : textTex(text, gds::sdl.getFont(gds::DEFAULT_FONT), SDL_Color{ 0xCC, 0x22, 0x33 }) {}
void Button::render() {
	textTex.render(pos);
}

void Button::trigger() {
	callback();
}

void Button::registerCallback(std::function<void()> func) {
	callback = func;
}

//------------- Selector

Selector::Selector(const std::string& label, const std::vector<std::string> options, int32_t initialIx)
	: label(label), options(options), selectionIx(initialIx) {
	for (const auto& opt : options) {
		textTexes.emplace_back(label + ": " + opt, gds::sdl.getFont(gds::DEFAULT_FONT), SDL_Color{ 0xCC, 0x22, 0x33 });
	}
}

void Selector::render() {
	textTexes[selectionIx].render(pos);
}

void Selector::trigger() {
	selectionIx = gds::positiveModulus(selectionIx + 1, options.size());
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
	w.setPosition(gds::addPoints(pos, cursor));

	cursor = gds::addPoints(cursor, {0, LINE_HEIGHT});
	return w;
}

Selector& MenuPage::addSelector(const std::string& label, const std::vector<std::string> options, int32_t initialIx) {
	widgets.push_back(std::make_unique<Selector>(label, options, initialIx));

	Selector& w = static_cast<Selector&>(*widgets.back());
	w.setPosition(gds::addPoints(pos, cursor));

	cursor = gds::addPoints(cursor, { 0, LINE_HEIGHT });
	return w;
}

gds::TextTexture& MenuPage::addTextTexture(const std::string& text, gds::Font& font, const SDL_Color& color, const SDL_Point& pos) {
	textures.emplace_back(gds::TextTexture{ text, font, color });
	texturePositions.push_back(pos);

	gds::TextTexture& tt = static_cast<gds::TextTexture&>(textures.back());
	return tt;
}

void MenuPage::render() {
	for (int ix = 0; ix < textures.size(); ++ix) {
		textures[ix].render(texturePositions[ix]);
	}

	for (auto& w : widgets)
		w->render();
	auto selectionIndicator = SDL_Rect{ pos.x - 2 * LINE_HEIGHT, pos.y + selectionIx * LINE_HEIGHT, LINE_HEIGHT, LINE_HEIGHT };

	SDL_SetRenderDrawColor(gds::sdl.renderer, 0xCC, 0x22, 0x33, 0xFF);
	SDL_RenderFillRect(gds::sdl.renderer, &selectionIndicator);
}

void MenuPage::handleKeys(SDL_Keycode key) {
	switch (key) {
	case SDLK_UP:
		selectionIx = gds::positiveModulus(selectionIx - 1, widgets.size());
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


//------------- Menu

Menu::Menu(MenuPage& mainPage) {
	pages.push(mainPage);
}

void Menu::pushPage(MenuPage& page) {
	pages.push(page);
}

void Menu::popPage() {
	assert(pages.size() > 1); // don't pop main page
	pages.pop();
}

MenuPage& Menu::peekPage() {
	assert(!pages.empty());
	return pages.top();
}

void Menu::render() {
	peekPage().render();
}

void Menu::handleKeys(SDL_Keycode key) {
	peekPage().handleKeys(key);
}

}
