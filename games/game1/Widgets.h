#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include <functional>
#include <memory>
#include <string>
#include <vector>

class Widget {
private:
	int x = -1;
	int y = -1;
public:
	virtual void render(SDL_Renderer* renderer, TTF_Font* font) = 0;
	virtual void trigger() = 0;
	void setPosition(int x, int y) { this->x = x; this->y = y; }
};

class Button : public Widget {
private:
	std::string text;
	std::function<void()> callback;
public:
	Button(const std::string& text);

	void render(SDL_Renderer* renderer, TTF_Font* font) final;
	void trigger() final;

	void registerCallback(std::function<void()> func);
};

class Menu {
private:
	std::vector<std::unique_ptr<Widget>> widgets;
	int32_t selectionIx;
	int x;
	int y;
public:
	Button& addButton(const std::string& text);

	// render every widget
	void render(SDL_Renderer* renderer, TTF_Font* font);

	// up/down for selecting widgets, enter to trigger them
	void handleKeys(SDL_Keycode key);
};