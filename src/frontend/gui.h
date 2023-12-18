#pragma once
#include <SDL2/SDL.h>
#include <core.h>
#include <stdatomic.h>
#include <nfd.h>

typedef struct {
  SDL_Renderer* renderer;
  SDL_Texture* texture;
	SDL_Window* window;
  unsigned int id; // OpenGL framebuffer texture ID
	nfdchar_t* rom_file;
  bool rom_loaded, running;
  SDL_Thread* emu_thread;
  atomic_bool emu_quit; 
  core_t core;
} gui_t;

void init_gui(gui_t* gui, const char* title);
void main_loop(gui_t* gui);
void destroy_gui(gui_t* gui);
void open_file(gui_t* gui);
void update_texture(gui_t* gui);
void reset(gui_t* gui);
void stop(gui_t* gui);
void start(gui_t* gui);