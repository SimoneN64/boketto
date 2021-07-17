#pragma once
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "file.h"

typedef struct {
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	SDL_Window* window;
	bool running;
} window_t;

void init_window(window_t* window, const char* title, int w, int h);
void main_loop(window_t* window, core_t* core);
void destroy_window(window_t* window);