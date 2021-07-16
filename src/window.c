#include "window.h"

void init_window(window_t* window, const char* title, int w, int h) {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	window = (window_t*)malloc(sizeof(window_t));
	if(window == NULL) {
		printf("Failed to allocate window struct\n");
		exit(-1);
	}

	window->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if(window->window == NULL) {
		printf("%s\n", SDL_GetError());
		exit(-2);
	}

	window->renderer = SDL_CreateRenderer(window->window, 0, SDL_RENDERER_ACCELERATED);
	if(window->renderer == NULL) {
		printf("%s\n", SDL_GetError());
		exit(-3);
	}

	window->texture = SDL_CreateTexture(window->renderer, SDL_PIXELFORMAT_ARGB1555, SDL_TEXTUREACCESS_STREAMING, GBA_W, GBA_H);
	if(window->texture == NULL) {
		printf("%s\n", SDL_GetError());
		exit(-4);
	}

	window->running = true;

	int err = SDL_RenderSetLogicalSize(window->renderer, GBA_W, GBA_H);
	if(err < 0) {
		printf("%s\n", SDL_GetError());
		exit(err);
	}
}

void main_loop(window_t* window, core_t* core) {
	while(window->running) {
		SDL_Event event;
		SDL_PollEvent(&event);
		switch(event.type) {
			case SDL_QUIT: window->running = false; break;
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym) {
					case SDLK_o: open_file(core); break;
				}
				break;
		}

		run_frame(core);

		SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 0);
		SDL_RenderClear(window->renderer);
		SDL_RenderPresent(window->renderer);

		SDL_Delay(1);
	}
}

void destroy_window(window_t* window) {
	SDL_DestroyRenderer(window->renderer);
	SDL_DestroyTexture(window->texture);
	SDL_DestroyWindow(window->window);
}