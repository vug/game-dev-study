#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include <functional>
#include <memory>
#include <string>
#include <vector>

class Widget {
protected:
	SDL_Point pos{};
public:

	virtual void render(SDL_Renderer* renderer, TTF_Font* font) = 0;

	virtual void trigger() = 0;

	void setPosition(const SDL_Point& p);
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

class Selector : public Widget {
private:
	std::string label;
	std::vector<std::string> options;
	int32_t selectionIx = 0;
	std::function<void()> callback;
public:
	Selector(const std::string& label, const std::vector<std::string> options);

	void render(SDL_Renderer* renderer, TTF_Font* font) final;
	void trigger() final;

	const std::string& getSelection() const;
	void registerCallback(std::function<void()> func);
};

class Menu {
private:
	std::vector<std::unique_ptr<Widget>> widgets;
	int32_t selectionIx = 0;
	SDL_Point pos;
	SDL_Point cursor = {0 ,0};
	const int LINE_HEIGHT = 20;
public:
	Menu(const SDL_Point& pos);

	Button& addButton(const std::string& text);
	Selector& addSelector(const std::string& label, const std::vector<std::string> options);

	// render every widget
	void render(SDL_Renderer* renderer, TTF_Font* font);

	// up/down for selecting widgets, enter to trigger them
	void handleKeys(SDL_Keycode key);
};