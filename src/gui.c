#include <capstone/capstone.h>
#include <core.h>
#include <gui.h>
#include <math.h>
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

  memset(gui->debugger.bgs[0], 0, DEPTH * GBA_W * GBA_H);

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
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GBA_W, GBA_H, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, framebuffer);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  
  glGenTextures(1, &gui->debugger.bg1id);
  glBindTexture(GL_TEXTURE_2D, gui->debugger.bg1id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GBA_W, GBA_H, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, gui->debugger.bgs[0]);
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

  for(int i = 0; i < GBA_W * GBA_H; i++) {
    u16 raw = *(u16*)&gui->core.mem.ppu.vram[i << 1];
    gui->debugger.bgs[0][i] = (color5_to_8(raw) << 24) | (color5_to_8(raw >> 5) << 16)
                            | (color5_to_8(raw >> 10) << 8) | 0xff;
  }

  glBindTexture(GL_TEXTURE_2D, gui->debugger.bg1id);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, GBA_W, GBA_H, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, gui->debugger.bgs[0]);
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
      if(igMenuItem_Bool("Stop", NULL, false, gui->rom_loaded)) {
        init_core(&gui->core);
        gui->rom_loaded = false;
        gui->rom_file = "";
      }
      igEndMenu();
    }
    igEndMainMenuBar();
  }
}

void disassembly(gui_t *gui) {
  u32 instructions[25] = {};
  u32 pc = gui->core.cpu.regs.gpr[PC];
  u32 pointer = pc - (14 * 4);
  pointer -= (pointer & 3); // align the pointer

  if(gui->rom_loaded) {
    for(int i = 0; i < 25; i++) {
      instructions[i] = read_32(&gui->core.mem, pointer + i * 4, pointer + i * 4);
    }
  } else {
    memset(instructions, 0xFFFFFFFF, 100);
  }

  u8 code[100];
  memcpy(code, instructions, 100);
  
  gui->debugger.count = cs_disasm(gui->debugger.handle, code, sizeof(code), pointer, 25, &gui->debugger.insn);
  igBegin("Disassembly", NULL, 0);
  if(gui->debugger.count > 0) {
    ImVec2 window_size;
    igGetWindowSize(&window_size);
    for(size_t j = 0; j < gui->debugger.count; j++) {
      const float font_size = igGetFontSize() * strlen(gui->debugger.insn[j].op_str) / 2;
      switch(j) {
      case 12 ... 14:
        igTextColored(colors_disasm[j & 3], "0x%"PRIx64":\t%s", gui->debugger.insn[j].address, gui->debugger.insn[j].mnemonic);
        igSameLine(window_size.x - font_size, -1);
        igTextColored(colors_disasm[j & 3], "%s", gui->debugger.insn[j].op_str);
        break;
      default:
        igText("0x%"PRIx64":\t%s", gui->debugger.insn[j].address, gui->debugger.insn[j].mnemonic);
        igSameLine(window_size.x - font_size, -1);
        igText("%s", gui->debugger.insn[j].op_str);
      }
    }

    cs_free(gui->debugger.insn, gui->debugger.count);
  } else {
    igText(gui->rom_loaded ? "Could not disassemble instruction!" : "No game loaded!");
  }
  igEnd();
}

void registers_view(gui_t *gui) {
  u32* regs = gui->core.cpu.regs.gpr;
  psr_t cpsr = gui->core.cpu.regs.cpsr;
  psr_t spsr = gui->core.cpu.regs.spsr;
  igBegin("Registers view", NULL, 0);
  for(int i = 0; i < 16; i+=4) {
    igText("r%d: %08X r%d: %08X r%d: %08X r%d: %08X", i, regs[i], i + 1, regs[i + 1], i + 2, regs[i + 2], i + 3, regs[i + 3]);
  }
  igText("CPSR: %08X", cpsr.raw);
  igText("SPSR: %08X", spsr.raw);
  igSeparator();
  igText("Current PSR state:");
  igText("Negative: %d Zero: %d Carry: %d Overflow: %d", cpsr.negative, cpsr.zero, cpsr.carry, cpsr.overflow);
  igText("IRQ Disable: %d FIQ Disable: %d", cpsr.irq, cpsr.fiq);
  igText("Thumb %d", cpsr.thumb);
  igText("Mode: %s", mode_str[cpsr.mode]);
  igSeparator();
  igText("Saved PSR state:");
  igText("Negative: %d Zero: %d Carry: %d Overflow: %d", spsr.negative, spsr.zero, spsr.carry, spsr.overflow);
  igText("IRQ Disable: %d FIQ Disable: %d", spsr.irq, spsr.fiq);
  igText("Thumb %d", spsr.thumb);
  igText("Mode: %s", mode_str[spsr.mode]);
  igEnd();
}

void vram_view(gui_t* gui) {
  igBegin("VRAM view", NULL, ImGuiWindowFlags_AlwaysAutoResize);
  ImVec2 vram_size = {.x = GBA_W * 1.5, .y = GBA_H * 1.5};
  igImage((ImTextureID)((intptr_t)gui->debugger.bg1id), vram_size, ZERO, ONE, FULL4, ZERO4);
  igEnd();
}

void debugger_window(gui_t* gui) {
  disassembly(gui);
  registers_view(gui);
  vram_view(gui);
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
    ImVec2 window_size;
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
  gui->rom_file = "";
	nfdfilteritem_t filter = { "GameBoy Advance roms", "gba" };
	nfdresult_t result = NFD_OpenDialog(&gui->rom_file, &filter, 1, "roms/");
	if(result == NFD_OKAY) {
    init_core(&gui->core);
    gui->rom_loaded = false;
		load_rom(&gui->core.mem, gui->rom_file);
    gui->core.running = true;
    gui->rom_loaded = true;
    flush_pipe_32(&gui->core.cpu.regs, &gui->core.mem);
	}
}