#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include <memory>
#include <string>
#include <unordered_map>

namespace gds {

class Result {
private:
	int val;
public:
	Result(int val);
	bool assertOK() const;
};

class Font;

class Sdl {
private:
	std::string name;
	int width{};
	int height{};
	std::unordered_map<std::string, Font> fonts;
public:
	SDL_Renderer* renderer;
	SDL_Window* window;
public:
	Sdl(const std::string& name, int width, int height);
	~Sdl();

	Font& loadFont(const std::string& name, const char* file, int size, int style = TTF_STYLE_NORMAL);
	const Font& getFont(const std::string& name) const;
	void renderPresent() const;
};

// Global Variable to be set in main function
extern Sdl sdl;
const std::string DEFAULT_FONT = "DEFAULT";

class Font {
private:
	TTF_Font* sdlFont = nullptr;
public:
	Font(const char* file, int size, int style = TTF_STYLE_NORMAL);
	~Font();

	TTF_Font* const get() const;
	bool isValid() const;
};

class Texture {
private:
	SDL_Texture* sdlTexture = nullptr;
	uint32_t format{};
	int access{};
	int width{};
	int height{};
public:
	Texture() = default;
	Texture(SDL_Texture* tex);
	Texture(const Texture& other) = delete;
	Texture& operator=(const Texture& other) = delete;
	Texture(Texture&& other) noexcept;
	Texture& operator=(Texture&& other) noexcept;

	~Texture();

	SDL_Texture* get() const;
	bool isValid() const;
	SDL_Point getSize() const;

	void render(const SDL_Point& pos);
};

void renderText(const std::string& text, SDL_Color color, int x, int y, TTF_Font* font, bool center = false);

int positiveModulus(int num, int mod);

SDL_Point addPoints(const SDL_Point& a, const SDL_Point& b);

}

