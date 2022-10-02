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

class Font {
public: // TODO: make private
	TTF_Font* sdlFont = nullptr;
public:
	Font(const char* file, int size, int style = TTF_STYLE_NORMAL);
	~Font();
	bool isValid() const;
};

class Texture {
private:
	const Sdl& sdl;
	SDL_Texture* sdlTexture = nullptr;
	uint32_t format{};
	int access{};
	int width{};
	int height{};
public:
	Texture(const Sdl& sdl, const Font& font, const std::string& text, const SDL_Color& color = { 0xFF, 0xFF, 0xFF });
	~Texture();
	bool isValid() const;

	void render(const SDL_Point& pos);
};

void renderText(const std::string& text, SDL_Color color, int x, int y, SDL_Renderer* renderer, TTF_Font* font, bool center = false);

int positiveModulus(int num, int mod);

SDL_Point addPoints(const SDL_Point& a, const SDL_Point& b);

}

