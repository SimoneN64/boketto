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
#include <capstone/capstone.h>

static const ImVec2 ZERO = {.x = 0, .y = 0};
static const ImVec2 ONE = {.x = 1, .y = 1};
static const ImVec2 MAX = {.x = __FLT_MAX__, .y = __FLT_MAX__};
static const ImVec4 ZERO4 = {.x = 0, .y = 0, .z = 0, .w = 0};
static const ImVec4 FULL4 = {.x = 1, .y = 1, .z = 1, .w = 1};
static const ImVec4 RED = {.x = 1, .y = 0, .z = 0, .w = 1};
static const ImVec4 ORANGE = {.x = 1, .y = 0.619, .z = 0, .w = 1};
static const ImVec4 YELLOW = {.x = 1, .y = 0.988, .z = 0, .w = 1};

typedef struct {
  core_t* core;
  csh handle;
  cs_insn* insn;
  size_t count;
} debugger_t;

typedef struct {
  ImGuiContext* ctx;
  ImGuiIO* io;
	GLFWwindow* window;
  unsigned int id; // OpenGL framebuffer texture ID
	nfdchar_t* rom_file;
  bool rom_loaded;
  core_t core;
  atomic_bool emu_quit; 
  pthread_t emu_thread_id;
  debugger_t debugger;
} gui_t;

void init_debugger(debugger_t* debugger, core_t* core);
void destroy_debugger(debugger_t* debugger);
void init_gui(gui_t* gui, const char* title);
void main_loop(gui_t* gui);
void destroy_gui(gui_t* gui);
void open_file(gui_t* gui);
void update_texture(gui_t* gui);
void main_menubar(gui_t* gui);
void debugger_window(gui_t* gui);
void disassembly(gui_t* gui);
void registers_view(gui_t* gui);