#include "window.h"
#include "log.h"

void init_window(window_t* window, const char* title, int w, int h) {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	window->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if(window->window == NULL) {
		logfatal("%s\n", SDL_GetError());
	}

	window->renderer = SDL_CreateRenderer(window->window, 0, SDL_RENDERER_ACCELERATED);
	if(window->renderer == NULL) {
    logfatal("%s\n", SDL_GetError());
	}

	window->texture = SDL_CreateTexture(window->renderer, SDL_PIXELFORMAT_ABGR1555, SDL_TEXTUREACCESS_STREAMING, GBA_W, GBA_H);
	if(window->texture == NULL) {
    logfatal("%s\n", SDL_GetError());
	}

	window->running = true;

	int err = SDL_RenderSetLogicalSize(window->renderer, GBA_W, GBA_H);
	if(err < 0) {
    logfatal("%s\n", SDL_GetError());
	}

	NFD_Init();
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

    SDL_UpdateTexture(window->texture, NULL, core->mem.ppu.framebuffer, GBA_W * DEPTH);
		SDL_RenderCopy(window->renderer, window->texture, NULL, NULL);
		SDL_RenderPresent(window->renderer);

		SDL_Delay(1);
	}
}

void destroy_window(window_t* window) {
	NFD_Quit();
	SDL_DestroyRenderer(window->renderer);
	SDL_DestroyTexture(window->texture);
	SDL_DestroyWindow(window->window);
	SDL_Quit();
}

void open_file(core_t* core) {
	nfdfilteritem_t filter[1] = {{ "GameBoy Advance roms", "gba" }};
	nfdresult_t result = NFD_OpenDialog(&core->rom_file, filter, 1, "roms/");
	if(result == NFD_OKAY) {
    init_core(core);
		load_rom(&core->mem, core->rom_file);
    flush_pipe_32(&core->cpu.regs, &core->mem);
		core->running = true;
	}
}