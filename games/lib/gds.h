#pragma once

#include <SDL.h>
#include <SDL_ttf.h>

#include <string>

void renderText(const std::string& text, SDL_Color color, int x, int y, SDL_Renderer* renderer, TTF_Font* font, bool center = false);

int positiveModulus(int num, int mod);