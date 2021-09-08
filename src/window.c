#include <window.h>
#include <log.h>

void init_window(core_t* core, window_t* window, const char* title) {
	if(glfwInit() == GLFW_FALSE) {
    logfatal("Couldn't initialize GLFW\n");
  }

  const char* glsl_version = "#version 330";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwSetErrorCallback(glfw_error_callback);

  const GLFWvidmode *details = glfwGetVideoMode(glfwGetPrimaryMonitor());
  int w = details->width - (details->width / 4), h = details->height - (details->height / 4);
  window->window = glfwCreateWindow(w, h, title, NULL, NULL);
  glfwSetWindowPos(window->window, details->width / 2 - w / 2, details->height / 2 - h / 2);
  glfwSetFramebufferSizeCallback(window->window, framebuffer_size_callback);

  glfwMakeContextCurrent(window->window);
  glfwSwapInterval(0);

  if(!gladLoadGL()) {
    logfatal("Failed to initialize OpenGL loader!\n");
  }

  ImFontAtlas* font_atlas = ImFontAtlas_ImFontAtlas();
  ImFont* font = ImFontAtlas_AddFontFromFileTTF(font_atlas, "include/external/resources/OpenSans-Regular.ttf", 16, NULL, NULL);

  window->ctx = igCreateContext(font_atlas);
  window->io = igGetIO();
  ImGui_ImplGlfw_InitForOpenGL(window->window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  ImGuiStyle* style = igGetStyle();
  style->WindowRounding = 10.0;
  igStyleColorsDark(NULL);

  glGenTextures(1, &window->id);
  glBindTexture(GL_TEXTURE_2D, window->id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, GBA_W, GBA_H, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, core->mem.ppu.framebuffer);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	NFD_Init();
}

ImVec2 image_size;

static void resize_callback(ImGuiSizeCallbackData* data) {
  ImVec2 window_size;
  igGetWindowSize(&window_size);
  float x = window_size.x - 15, y = window_size.y - 15;
  float current_aspect_ratio = x / y;
  
  if((GBA_W / GBA_H) > current_aspect_ratio) {
    y = x / (GBA_W / GBA_H);
  } else {
    x = y * (GBA_W / GBA_H);
  }

  image_size.x = x;
  image_size.y = y;
}

void update_texture(window_t* window, core_t* core) {
  glBindTexture(GL_TEXTURE_2D, window->id);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, GBA_W, GBA_H, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, core->mem.ppu.framebuffer);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}

void main_loop(window_t* window, core_t* core) {
	while (!glfwWindowShouldClose(window->window)) {
		glfwPollEvents();

		run_frame(core);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    igNewFrame();

    static const ImVec2 ZERO = {.x = 0, .y = 0};
    static const ImVec4 ZERO4 = {.x = 0, .y = 0, .z = 0, .w = 0};
    static const ImVec2 MAX = {.x = __FLT_MAX__, .y = __FLT_MAX__};
    igSetNextWindowSizeConstraints(ZERO, MAX, resize_callback, NULL);
    igBegin("Display", NULL, 0);
    igImage((ImTextureID)window->id, image_size, ZERO, MAX, ZERO4, ZERO4);
    igEnd();
    
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    
    igRender();
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());

    glfwSwapBuffers(window->window);
	}
}

void destroy_window(window_t* window) {
  NFD_Quit();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  igDestroyContext(window->ctx);
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