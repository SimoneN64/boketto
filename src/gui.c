#include <capstone.h>
#include <core.h>
#include <gui.h>
#include <math.h>
#include <log.h>

INLINE void* core_callback(void* vpargs) {
  gui_t* gui = (gui_t*)vpargs;
  while(!atomic_load(&gui->emu_quit)) {
    run_frame(&gui->core);
  }

  return NULL;
}

void init_gui(gui_t* gui, const char* title) {
  init_core(&gui->core);
  init_disasm(&gui->debugger);

	if(SDL_Init(SDL_INIT_VIDEO) != 0) {
    logfatal("Error: %s\n", SDL_GetError());
  }

  gui->rom_loaded = false;
  gui->running = true;

  const char* glsl_version = "#version 330";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  SDL_DisplayMode details;
  SDL_GetCurrentDisplayMode(0, &details);
  
  int w = details.w - (details.w / 4), h = details.h - (details.h / 4);
  
  gui->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
  gui->gl_context = SDL_GL_CreateContext(gui->window);
  SDL_GL_MakeCurrent(gui->window, gui->gl_context);

  ImFontAtlas* firacode_font_atlas = ImFontAtlas_ImFontAtlas();
  ImFont* firacode_font = ImFontAtlas_AddFontFromFileTTF(firacode_font_atlas, "resources/FiraCode-VariableFont_wght.ttf", 16, NULL, NULL);

  gui->ctx = igCreateContext(firacode_font_atlas);
  gui->io = igGetIO();

  ImGui_ImplSDL2_InitForOpenGL(gui->window, gui->gl_context);
  ImGui_ImplOpenGL3_Init(glsl_version);

  ImGuiStyle* style = igGetStyle();
  style->WindowRounding = 10.0;
  igStyleColorsDark(NULL);

  uint8_t current_framebuffer = gui->core.mem.ppu.current_framebuffer ^ 1;
  u16* framebuffer = gui->core.mem.ppu.framebuffers[current_framebuffer];
  
  glGenTextures(1, &gui->id);
  glBindTexture(GL_TEXTURE_2D, gui->id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GBA_W, GBA_H, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, framebuffer);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	NFD_Init();

  //pthread_create(&gui->emu_thread_id, NULL, core_callback, (void*)gui);
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

void main_loop(gui_t* gui) {
  ImGuiIO* io = igGetIO();
  ImVec4 clear_color = {0.45f, 0.55f, 0.60f, 1.00f};
  while (gui->running) {
    update_texture(gui);
    
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);

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

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    igNewFrame();
    
    main_menubar(gui);
    debugger_window(gui);

    run_frame(&gui->core);
    
    igSetNextWindowSizeConstraints(ZERO, MAX, resize_callback, NULL);
    igBegin("Display", NULL, ImGuiWindowFlags_NoTitleBar);
    ImVec2 window_size;
    igGetWindowSize(&window_size);
    ImVec2 result = {.x = (window_size.x - image_size.x) * 0.5, .y = (window_size.y - image_size.y) * 0.5};
    igSetCursorPos(result);
    igImage((ImTextureID)((intptr_t)gui->id), image_size, ZERO, ONE, FULL4, ZERO4);
    igEnd();
    
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    
    igRender();
    glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
    SDL_GL_SwapWindow(gui->window);
  }
}

void update_texture(gui_t* gui) {
  u8 current_framebuffer = atomic_load(&gui->core.mem.ppu.current_framebuffer) ^ 1;
  u16* framebuffer = gui->core.mem.ppu.framebuffers[current_framebuffer];

  glBindTexture(GL_TEXTURE_2D, gui->id);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, GBA_W, GBA_H, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, framebuffer);
}

void main_menubar(gui_t *gui) {
  if(igBeginMainMenuBar()) {
    if(igBeginMenu("File", true)) {
      if(igMenuItem_Bool("Open", "O", false, true)) {
        open_file(gui);
      }
      if(igMenuItem_Bool("Exit", "Esc", false, true)) {
        gui->running = false;
      }
      igEndMenu();
    }
    if(igBeginMenu("Emulation", true)) {
      char* pause_text = "Pause";
      if(!gui->core.running && gui->rom_loaded) {
        pause_text = "Resume";
      }

      if(igMenuItem_Bool(pause_text, "P", false, gui->rom_loaded)) {
        gui->core.running = !gui->core.running;
      }
      if(igMenuItem_Bool("Stop", NULL, false, gui->rom_loaded)) {
        stop(gui);
      }
      if(igMenuItem_Bool("Reset", NULL, false, gui->rom_loaded)) {
        reset(gui);
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

void debugger_window(gui_t* gui) {
  disassembly(gui);
  registers_view(gui);
}

void destroy_gui(gui_t* gui) {
  gui->emu_quit = true;
  //pthread_join(gui->emu_thread_id, NULL);
  destroy_disasm(&gui->debugger);
  NFD_Quit();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  igDestroyContext(gui->ctx);
  SDL_GL_DeleteContext(gui->gl_context);
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
  gui->emu_quit = !gui->rom_loaded;
  gui->core.running = gui->rom_loaded;
  if(gui->rom_loaded) {
    //pthread_create(&gui->emu_thread_id, NULL, core_callback, (void*)gui);
  }
}

void reset(gui_t* gui) {
  stop(gui);
  start(gui);
}

void stop(gui_t* gui) {
  gui->emu_quit = true;
  //pthread_join(gui->emu_thread_id, NULL);
  init_core(&gui->core);
  gui->rom_loaded = false;
  gui->core.running = false;
}