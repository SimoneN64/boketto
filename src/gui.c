#include <gui.h>
#include <log.h>

static void glfw_error_callback(int error, const char* description) {
  logfatal("Glfw Error %d: %s\n", error, description);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void* core_cb(void* vpargs) {
  gui_t* gui = (gui_t*)vpargs;
  while(!atomic_load(&gui->emu_quit)) {
    run_frame(&gui->core);
  }

  return NULL;
}

void init_gui(gui_t* gui, const char* title) {
	if(glfwInit() == GLFW_FALSE) {
    logfatal("Couldn't initialize GLFW\n");
  }

  const char* glsl_version = "#version 330";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwSetErrorCallback(glfw_error_callback);

  const GLFWvidmode *details = glfwGetVideoMode(glfwGetPrimaryMonitor());
  int w = details->width - (details->width / 4), h = details->height - (details->height / 4);
  gui->window = glfwCreateWindow(w, h, title, NULL, NULL);
  glfwSetWindowPos(gui->window, details->width / 2 - w / 2, details->height / 2 - h / 2);
  glfwSetFramebufferSizeCallback(gui->window, framebuffer_size_callback);

  glfwMakeContextCurrent(gui->window);
  glfwSwapInterval(0);

  if(!gladLoadGL()) {
    logfatal("Failed to initialize OpenGL loader!\n");
  }

  ImFontAtlas* font_atlas = ImFontAtlas_ImFontAtlas();
  ImFont* font = ImFontAtlas_AddFontFromFileTTF(font_atlas, "resources/OpenSans-Regular.ttf", 16, NULL, NULL);

  gui->ctx = igCreateContext(font_atlas);
  gui->io = igGetIO();
  ImGui_ImplGlfw_InitForOpenGL(gui->window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  ImGuiStyle* style = igGetStyle();
  style->WindowRounding = 10.0;
  igStyleColorsDark(NULL);
  
  init_core(&gui->core);
  uint8_t current_framebuffer = gui->core.mem.ppu.current_framebuffer ^ 1;
  u32* framebuffer = gui->core.mem.ppu.framebuffers[current_framebuffer];
  
  glGenTextures(1, &gui->id);
  glBindTexture(GL_TEXTURE_2D, gui->id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, GBA_W, GBA_H, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, framebuffer);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	NFD_Init();

  pthread_create(&gui->emu_thread_id, NULL, core_cb, (void*)gui);
}

ImVec2 image_size;

static void resize_callback(ImGuiSizeCallbackData* data) {
  ImVec2 window_size;
  igGetWindowSize(&window_size);
  float x = window_size.x - 15, y = window_size.y - 15;
  float current_aspect_ratio = x / y;
  
  if(((float)GBA_W / GBA_H) > current_aspect_ratio) {
    y = x / ((float)GBA_W / GBA_H);
  } else {
    x = y * ((float)GBA_W / GBA_H);
  }
  image_size.x = x;
  image_size.y = y;
}

void update_texture(gui_t* gui) {
  u8 current_framebuffer = atomic_load(&gui->core.mem.ppu.current_framebuffer) ^ 1;
  u32* framebuffer = gui->core.mem.ppu.framebuffers[current_framebuffer];

  glBindTexture(GL_TEXTURE_2D, gui->id);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, GBA_W, GBA_H, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, framebuffer);
}

void main_loop(gui_t* gui) {
  while (!glfwWindowShouldClose(gui->window)) {
    update_texture(gui);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    igNewFrame();

    if (glfwGetKey(gui->window, GLFW_KEY_O) == GLFW_PRESS) {
      open_file(gui);
    }
    
    if(igBeginMainMenuBar()) {
      if(igBeginMenu("File", true)) {
        if(igMenuItem_Bool("Open", "O", false, true)) {
          open_file(gui);
        }
        igEndMenu();
      }
      igEndMainMenuBar();
    }

    static const ImVec2 ZERO = {.x = 0, .y = 0};
    static const ImVec4 ZERO4 = {.x = 0, .y = 0, .z = 0, .w = 0};
    static const ImVec2 MAX = {.x = __FLT_MAX__, .y = __FLT_MAX__};
    igSetNextWindowSizeConstraints(ZERO, MAX, resize_callback, NULL);
    igBegin("Display", NULL, 0);
    igImage((ImTextureID)gui->id, image_size, ZERO, MAX, ZERO4, ZERO4);
    igEnd();
    
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.227, 0.345, 0.454, 1.00);
    
    igRender();
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());

    glfwSwapBuffers(gui->window);
    glfwPollEvents();
  }
}

void destroy_gui(gui_t* gui) {
  gui->emu_quit = true;
  pthread_join(gui->emu_thread_id, NULL);
  destroy_core(&gui->core);
  NFD_Quit();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  igDestroyContext(gui->ctx);
  glfwDestroyWindow(gui->window);
}

void open_file(gui_t* gui) {
	nfdfilteritem_t filter = { "GameBoy Advance roms", "gba" };
	nfdresult_t result = NFD_OpenDialog(&gui->rom_file, &filter, 1, "roms/");
	if(result == NFD_OKAY) {
    destroy_core(&gui->core);
    init_core(&gui->core);
		load_rom(&gui->core.mem, gui->rom_file);
    flush_pipe_32(&gui->core.cpu.regs, &gui->core.mem);
		gui->core.running = true;
	}
}