#pragma once
#include <glad.h>
#ifdef _WIN32
#include <glfw/glfw3.h>
#else
#include <GLFW/glfw3.h>
#endif
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cimgui_impl.h>
#include <pthread.h>
#include <core.h>
#include <stdatomic.h>

typedef struct {
  ImGuiContext* ctx;
  ImGuiIO* io;
	GLFWwindow* window;
  unsigned int id; // OpenGL framebuffer texture ID
	nfdchar_t* rom_file;
  core_t core;
  atomic_bool emu_quit; 
  pthread_t emu_thread_id;
} gui_t;

void init_gui(gui_t* window, const char* title);
void main_loop(gui_t* gui);
void destroy_gui(gui_t* gui);
void open_file(gui_t* gui);
void update_texture(gui_t* gui);