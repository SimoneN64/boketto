#include <core.h>
#include <gui.h>
#include <math.h>
#include <log.h>

INLINE int core_callback(void* vpargs) {
  gui_t* gui = (gui_t*)vpargs;
  while(!atomic_load(&gui->emu_quit)) {
    run_frame(&gui->core);
  }

  return 0;
}

void init_gui(gui_t* gui, const char* title) {
  init_core(&gui->core);

	if(SDL_Init(SDL_INIT_VIDEO) != 0) {
    logfatal("Error: %s\n", SDL_GetError());
  }

  gui->rom_loaded = false;
  gui->running = true;
  
  gui->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  gui->renderer = SDL_CreateRenderer(gui->window, -1, SDL_RENDERER_ACCELERATED);
  SDL_RenderSetLogicalSize(gui->renderer, GBA_W, GBA_H);

  gui->texture = SDL_CreateTexture(gui->renderer, SDL_PIXELFORMAT_BGR555, SDL_TEXTUREACCESS_STREAMING, GBA_W, GBA_H);

	NFD_Init();

  gui->emu_thread = SDL_CreateThread(core_callback, NULL, (void*)gui);
}

void main_loop(gui_t* gui) {
  while (gui->running) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_QUIT: gui->running = false; break;
        case SDL_WINDOWEVENT:
          if(event.window.event == SDL_WINDOWEVENT_CLOSE) {
            gui->running = false;
          }
          break;  
        case SDL_KEYDOWN:
          switch(event.key.keysym.sym) {
            case SDLK_o: open_file(gui); break;
            case SDLK_p:
              if(gui->rom_loaded) {
                gui->core.running = !gui->core.running;
              }
              break;
            case SDLK_ESCAPE: gui->running = false; break;
          }
          break;
      }
    }

    update_texture(gui);
    SDL_RenderClear(gui->renderer);
    SDL_RenderCopy(gui->renderer, gui->texture, NULL, NULL);
    SDL_RenderPresent(gui->renderer);
  }
}

void update_texture(gui_t* gui) {
  u8 current_framebuffer = atomic_load(&gui->core.mem.ppu.current_framebuffer) ^ 1;
  u16* framebuffer = gui->core.mem.ppu.framebuffers[current_framebuffer];

  SDL_UpdateTexture(gui->texture, NULL, framebuffer, GBA_W * sizeof(u16));
}

void destroy_gui(gui_t* gui) {
  gui->emu_quit = true;
  SDL_DetachThread(gui->emu_thread);
  NFD_Quit();
  SDL_DestroyTexture(gui->texture);
  SDL_DestroyRenderer(gui->renderer);
  SDL_DestroyWindow(gui->window);
  SDL_Quit();
}

void open_file(gui_t* gui) {
	nfdfilteritem_t filter = { "GameBoy Advance roms", "gba" };
	nfdresult_t result = NFD_OpenDialog(&gui->rom_file, &filter, 1, "roms/");
	if(result == NFD_OKAY) {
    reset(gui);
	}
}

void start(gui_t* gui) {
  gui->rom_loaded = load_rom(&gui->core.mem, gui->rom_file);
  flush_pipe_32(&gui->core.cpu.regs, &gui->core.mem);
  gui->emu_quit = !gui->rom_loaded;
  gui->core.running = gui->rom_loaded;
  if(gui->rom_loaded) {
    gui->emu_thread = SDL_CreateThread(core_callback, NULL, (void*)gui);
  }
}

void reset(gui_t* gui) {
  stop(gui);
  start(gui);
}

void stop(gui_t* gui) {
  gui->emu_quit = true;
  SDL_DetachThread(gui->emu_thread);
  init_core(&gui->core);
  gui->rom_loaded = false;
  gui->core.running = false;
}