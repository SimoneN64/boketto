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
#include <core.h>

typedef struct {
  ImGuiContext* ctx;
  ImGuiIO* io;
	GLFWwindow* window;
  unsigned int id;
} window_t;

void init_window(core_t* core, window_t* window, const char* title);
void main_loop(window_t* window, core_t* core);
void destroy_window(window_t* window);
void open_file(core_t* core);
void update_texture(window_t* window, core_t* core);

static inline void glfw_error_callback(int error, const char* description) {
  logfatal("Glfw Error %d: %s\n", error, description);
}

static inline void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}