#include "gds.h"

#include <iostream>

namespace gds {

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
