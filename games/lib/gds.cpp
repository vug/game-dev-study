#include "gds.h"

#include <cassert>
#include <iostream>

namespace gds {

//------------- Result

Result::Result(int val) : val(val) {}

bool Result::assertOK() const {
	bool isOK = val == 0;
	if (!isOK) {
		std::cerr << SDL_GetError() << "\n";
		assert(false);
	}
	return isOK;
}

//------------- Sdl

Sdl::Sdl(const std::string& name, int width, int height) : name(name), width(width), height(height) {
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
	//SDL_Surface* gScreenSurface = SDL_GetWindowSurface(gWindow);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	// Should be able to render into textures
	SDL_RendererInfo info{};
	SDL_GetRendererInfo(renderer, &info);
	assert(info.flags & SDL_RENDERER_TARGETTEXTURE);
}

Sdl::~Sdl() {
	// Since Sdl has unordered_map fonts as a member, which owns the fonts. Members are destroyed after the destructor call.
	// But ~Font() calls TTF_CloseFont(), which needs to be called before TTF_Quit() (otherwise it throws). So, delete loaded fonts before TTF_Quit().
	fonts.clear();
	TTF_Quit();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

Font& Sdl::loadFont(const std::string& name, const char* file, int size, int style) {
	assert(!fonts.contains(name)); // can't emplace a font with the same name
	//auto [it, hasEmplaced] = fonts.emplace(name, Font{ file, size, style });
	auto [it, hasEmplaced] = fonts.try_emplace(name, file, size, style );
	assert(hasEmplaced);
	return it->second;
}

Font& Sdl::getFont(const std::string& name) {
	assert(fonts.contains(name)); // don't get a font that's not loaded
	return fonts.at(name);
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
	if (sdlFont != nullptr)
		TTF_CloseFont(sdlFont);
}

TTF_Font* const Font::get() const {
	return sdlFont;
}

bool Font::isValid() const {
	return sdlFont != nullptr;
}


//------------- Texture

Texture::Texture(SDL_Texture* tex) : sdlTexture(tex) {
	SDL_QueryTexture(sdlTexture, &format, &access, &width, &height);
}

Texture::Texture(Texture&& other) noexcept 
	: sdlTexture(other.sdlTexture), format(other.format), access(other.access), width(other.width), height(other.height) {
	other.sdlTexture = nullptr;
	other.format = {};
	other.access = {};
	other.width = {};
	other.height = {};
}

Texture& Texture::operator=(Texture&& other) noexcept {
	if (this == &other) 
		return *this;

	if (sdlTexture != nullptr)
		SDL_DestroyTexture(sdlTexture);
	sdlTexture = other.sdlTexture;
	format = other.format;
	access = other.access;
	width = other.width;
	height = other.height;

	other.sdlTexture = nullptr;
	other.format = {};
	other.access = {};
	other.width = {};
	other.height = {};
	return *this;
}

Texture::~Texture() {
	if (sdlTexture != nullptr)
		SDL_DestroyTexture(sdlTexture);
}

SDL_Texture* Texture::get() const {
	return sdlTexture;
}

bool Texture::isValid() const {
	return sdlTexture != nullptr;
}

SDL_Point Texture::getSize() const {
	return { width, height };
}

void Texture::render(const SDL_Point& pos) {
	SDL_Rect dstRect{ pos.x, pos.y, width, height };
	SDL_RenderCopy(gds::sdl.renderer, sdlTexture, NULL, &dstRect);
}


//------------- TextTexture

SDL_Texture* TextTexture::makeTexture(const std::string& text, const Font& font, const SDL_Color& color) {
	SDL_Surface* textSurface = TTF_RenderText_Blended(font.get(), text.c_str(), color);   // anti-aliased glyphs, TTF_RenderText_Solid() for aliased glyphs
	if (textSurface == nullptr)
		std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << "\n";
	return SDL_CreateTextureFromSurface(gds::sdl.renderer, textSurface);
}

TextTexture::TextTexture(const std::string& text, Font& font, const SDL_Color& color)
	: text(text), font(font), color(color), Texture(makeTexture(text, font, color)) {}

void TextTexture::setText(const std::string & text) {
	this->text = text;
	sdlTexture = makeTexture(text, font, color);
	SDL_QueryTexture(sdlTexture, &format, &access, &width, &height);
}


//-------------

void renderText(const std::string& text, SDL_Color color, int x, int y, TTF_Font* font, bool center) {
	SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), color);   // anti-aliased glyphs, TTF_RenderText_Solid() for aliased glyphs
	if (textSurface == nullptr)
		std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << "\n";
	SDL_Texture* texture = SDL_CreateTextureFromSurface(gds::sdl.renderer, textSurface);

	int texW, texH;
	SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
	SDL_Rect dstrect = center ? SDL_Rect{ x - texW / 2, y - texH / 2, texW, texH } : SDL_Rect{ x, y, texW, texH };
	SDL_RenderCopy(gds::sdl.renderer, texture, NULL, &dstrect);

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
