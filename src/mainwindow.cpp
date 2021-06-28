#include "mainwindow.h"

namespace boketto::frontend
{
MainWindow::~MainWindow() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyTexture(texture);
  SDL_DestroyWindow(window);
  SDL_Quit();
  NFD_Quit();
}

MainWindow::MainWindow(std::string title) : file("config.ini") {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, GBA_WIDTH * 3, GBA_HEIGHT * 3, SDL_WINDOW_RESIZABLE);
  renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  SDL_RenderSetLogicalSize(renderer, GBA_WIDTH, GBA_HEIGHT);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR1555, SDL_TEXTUREACCESS_STREAMING, GBA_WIDTH, GBA_HEIGHT);

  if(!file.read(ini)) {
    ini["emulator"]["bios"] = "bios.bin";
    file.generate(ini);
  }

  core = std::make_unique<Core>(ini["emulator"]["bios"]);
  NFD_Init();
}

void MainWindow::OpenFile() {
  nfdchar_t *outpath;
  nfdfilteritem_t filteritem[2] = {{ "Game Boy roms", "gb" }, { "Game Boy Color roms", "gbc" }};
  nfdresult_t result = NFD_OpenDialog(&outpath, filteritem, 2, "roms/");
  if(result == NFD_OKAY)
  {
    core->LoadROM(std::string(outpath));
  }
}

void MainWindow::Run() {
  bool running = true;
  while(running) {
    SDL_Event event;
    SDL_PollEvent(&event);
    switch(event.type) {
      case SDL_QUIT: running = false; break;
      case SDL_KEYDOWN:
      switch(event.key.keysym.sym) {
        case SDLK_o: OpenFile(); break;
        case SDLK_q: running = false; break;
      }
    }

    core->run();

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
  }
}
}