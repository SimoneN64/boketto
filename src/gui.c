#include <capstone/capstone.h>
#include <core.h>
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

  gui->rom_loaded = false;

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

  ImFontAtlas* opensans_font_atlas = ImFontAtlas_ImFontAtlas();
  ImFont* opensans_font = ImFontAtlas_AddFontFromFileTTF(opensans_font_atlas, "resources/FiraCode-VariableFont_wght.ttf", 16, NULL, NULL);

  gui->ctx = igCreateContext(opensans_font_atlas);
  gui->io = igGetIO();

  ImGui_ImplGlfw_InitForOpenGL(gui->window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  ImGuiStyle* style = igGetStyle();
  style->WindowRounding = 10.0;
  igStyleColorsDark(NULL);
  
  init_core(&gui->core);
  init_debugger(&gui->debugger, &gui->core);

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

void init_debugger(debugger_t* debugger, core_t* core) {
  debugger->core = core;
  if(cs_open(CS_ARCH_ARM, CS_MODE_ARM | CS_MODE_LITTLE_ENDIAN, &debugger->handle) != CS_ERR_OK) {
    logfatal("Could not initialize capstone\n");
  }
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

void main_menubar(gui_t *gui) {
  if(igBeginMainMenuBar()) {
    if(igBeginMenu("File", true)) {
      if(igMenuItem_Bool("Open", "O", false, true)) {
        open_file(gui);
      }
      if(igMenuItem_Bool("Exit", "Esc", false, true)) {
        glfwSetWindowShouldClose(gui->window, GLFW_TRUE);
      }
      igEndMenu();
    }
    if(igBeginMenu("Emulation", true)) {
      if(igMenuItem_Bool(gui->core.running ? "Pause" : "Resume", "P", false, gui->rom_loaded)) {
        gui->core.running = !gui->core.running;
      }
      igEndMenu();
    }
    igEndMainMenuBar();
  }
}

void disassembly(gui_t *gui) {
  u32 instructions[25];
  if(gui->rom_loaded) {
    u32 pointer = gui->core.cpu.regs.gpr[PC] - 14 * 4;
    for(int i = 0; i < 25; i++) {
      instructions[i] = read_32(&gui->core.mem, pointer + i * 4, pointer + i * 4);
    }
  } else {
    memset(instructions, 0xFFFFFFFF, 25 * 4);
  }
  
  u8 code[100];

  memcpy(code, instructions, 100);
  gui->debugger.count = cs_disasm(gui->debugger.handle, code, sizeof(code), gui->core.cpu.regs.gpr[PC], 0, &gui->debugger.insn);
  igBegin("Disassembly", NULL, 0);
  if(gui->debugger.count > 0) {
    for(size_t j = 0; j < gui->debugger.count; j++) {
      switch(j) {
      case 12:
        igTextColored(RED, "0x%"PRIx64":\t%s\t\t%s", gui->debugger.insn[j].address, gui->debugger.insn[j].mnemonic, gui->debugger.insn[j].op_str);
        break;
      case 13:
        igTextColored(ORANGE, "0x%"PRIx64":\t%s\t\t%s", gui->debugger.insn[j].address, gui->debugger.insn[j].mnemonic, gui->debugger.insn[j].op_str);
        break;
      case 14:
        igTextColored(YELLOW, "0x%"PRIx64":\t%s\t\t%s", gui->debugger.insn[j].address, gui->debugger.insn[j].mnemonic, gui->debugger.insn[j].op_str);
        break;
      default:
        igText("0x%"PRIx64":\t%s\t\t%s", gui->debugger.insn[j].address, gui->debugger.insn[j].mnemonic, gui->debugger.insn[j].op_str);
      }
    }

    cs_free(gui->debugger.insn, gui->debugger.count);
  } else {
    igText(gui->rom_loaded ? "Could not disassemble instruction!" : "No game loaded!");
  }
  igEnd();
}

void registers_view(gui_t *gui) {
  igBegin("Registers view", NULL, 0);
  igText("r0:  %08X r1: %08X r2:  %08X r3:  %08X", gui->core.cpu.regs.gpr[0], gui->core.cpu.regs.gpr[1], gui->core.cpu.regs.gpr[2], gui->core.cpu.regs.gpr[3]);
  igText("r4:  %08X r5: %08X r6:  %08X r7:  %08X", gui->core.cpu.regs.gpr[4], gui->core.cpu.regs.gpr[5], gui->core.cpu.regs.gpr[6], gui->core.cpu.regs.gpr[7]);
  igText("r8:  %08X r9: %08X r10: %08X r11: %08X", gui->core.cpu.regs.gpr[8], gui->core.cpu.regs.gpr[9], gui->core.cpu.regs.gpr[10], gui->core.cpu.regs.gpr[11]);
  igText("r12: %08X sp: %08X lr:  %08X pc:  %08X", gui->core.cpu.regs.gpr[12], gui->core.cpu.regs.gpr[13], gui->core.cpu.regs.gpr[LR], gui->core.cpu.regs.gpr[PC]);
  igEnd();
}

void debugger_window(gui_t* gui) {
  disassembly(gui);
  registers_view(gui);
}

void main_loop(gui_t* gui) {
  while (!glfwWindowShouldClose(gui->window)) {
    update_texture(gui);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    igNewFrame();

    if (glfwGetKey(gui->window, GLFW_KEY_O) == GLFW_PRESS) {
      open_file(gui);
    } else if (glfwGetKey(gui->window, GLFW_KEY_P) == GLFW_PRESS && gui->rom_loaded) {
      gui->core.running = !gui->core.running;
    } else if (glfwGetKey(gui->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(gui->window, GLFW_TRUE);
    }
    
    main_menubar(gui);
    debugger_window(gui);
    
    igSetNextWindowSizeConstraints(ZERO, MAX, resize_callback, NULL);
    igBegin("Display", NULL, ImGuiWindowFlags_NoTitleBar);
    ImVec2 window_size, title_bar_size;
    igGetWindowSize(&window_size);
    ImVec2 result = {.x = (window_size.x - image_size.x) * 0.5, .y = (window_size.y - image_size.y) * 0.5};
    igSetCursorPos(result);
    igImage((ImTextureID)((intptr_t)gui->id), image_size, ZERO, ONE, FULL4, ZERO4);
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
  destroy_debugger(&gui->debugger);
  NFD_Quit();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  igDestroyContext(gui->ctx);
  glfwDestroyWindow(gui->window);
}

void destroy_debugger(debugger_t* debugger) {
  cs_close(&debugger->handle);
}

void open_file(gui_t* gui) {
	nfdfilteritem_t filter = { "GameBoy Advance roms", "gba" };
	nfdresult_t result = NFD_OpenDialog(&gui->rom_file, &filter, 1, "roms/");
	if(result == NFD_OKAY) {
    destroy_core(&gui->core);
    init_core(&gui->core);
		load_rom(&gui->core.mem, gui->rom_file);
    gui->core.running = true;
    gui->rom_loaded = true;
    flush_pipe_32(&gui->core.cpu.regs, &gui->core.mem);
	}
}