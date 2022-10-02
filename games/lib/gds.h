#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include <memory>
#include <string>

namespace gds {

class Sdl {
private:
	std::string name;
	int width{};
	int height{};
public:
	SDL_Renderer* renderer;
	SDL_Window* window;
public:
	Sdl(const std::string& name, int width, int height);
	~Sdl();

	void renderPresent() const;
};

void renderText(const std::string& text, SDL_Color color, int x, int y, SDL_Renderer* renderer, TTF_Font* font, bool center = false);

int positiveModulus(int num, int mod);

SDL_Point addPoints(const SDL_Point& a, const SDL_Point& b);

}

