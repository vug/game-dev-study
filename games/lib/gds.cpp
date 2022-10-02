#include "gds.h"

#include <iostream>

namespace gds {

//------------- Sdl

Sdl::Sdl(const std::string& name, int width, int height) : name(name), width(width), height(height) {
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
	//SDL_Surface* gScreenSurface = SDL_GetWindowSurface(gWindow);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

Sdl::~Sdl() {
	TTF_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void Sdl::renderPresent() const {
	SDL_RenderPresent(renderer);
}


//------------- Font

Font::Font(const char* file, int size, int style) {
	sdlFont = TTF_OpenFont(file, size);
	if (sdlFont == nullptr)
		std::cerr << "Font not found! " << TTF_GetError() << "\n";
	TTF_SetFontStyle(sdlFont, style);
}

Font::~Font() {
	TTF_CloseFont(sdlFont);
}

bool Font::isValid() const {
	return sdlFont != nullptr;
}


//------------- Texture

Texture::~Texture() {
	SDL_DestroyTexture(sdlTexture);
}

bool Texture::isValid() const {
	return sdlTexture != nullptr;
}

Texture::Texture(const Sdl& sdl, const Font& font, const std::string& text, const SDL_Color& color) 
	: sdl(sdl) {

	SDL_Surface* textSurface = TTF_RenderText_Blended(font.sdlFont, text.c_str(), color); // anti-aliased glyphs, TTF_RenderText_Solid() for aliased glyphs
	sdlTexture = SDL_CreateTextureFromSurface(sdl.renderer, textSurface);
	SDL_QueryTexture(sdlTexture, &format, &access, &width, &height);
	SDL_FreeSurface(textSurface);
}

void Texture::render(const SDL_Point& pos) {
	SDL_Rect dstRect{ pos.x, pos.y, width, height };
	SDL_RenderCopy(sdl.renderer, sdlTexture, NULL, &dstRect);
}


void renderText(const std::string& text, SDL_Color color, int x, int y, SDL_Renderer* renderer, TTF_Font* font, bool center) {
	SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), color);   // anti-aliased glyphs, TTF_RenderText_Solid() for aliased glyphs
	if (textSurface == nullptr)
		std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << "\n";
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);

	int texW, texH;
	SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
	SDL_Rect dstrect = center ? SDL_Rect{ x - texW / 2, y - texH / 2, texW, texH } : SDL_Rect{ x, y, texW, texH };
	SDL_RenderCopy(renderer, texture, NULL, &dstrect);

	SDL_DestroyTexture(texture);
	SDL_FreeSurface(textSurface);
}

int positiveModulus(int num, int mod) {
	int result = num % mod;
	if (result < 0)
		result += mod;
	return result;
}

SDL_Point addPoints(const SDL_Point& a, const SDL_Point& b) {
	return { a.x + b.x, a.y + b.y };
}

}
