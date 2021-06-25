#include "mainwindow.h"

namespace boketto::frontend
{
MainWindow::MainWindow(std::string title)
{
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
  window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, GBA_WIDTH * 3, GBA_HEIGHT * 3, SDL_WINDOW_RESIZABLE);
  renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  SDL_RenderSetLogicalSize(renderer, GBA_WIDTH, GBA_HEIGHT);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR1555, SDL_TEXTUREACCESS_STREAMING, GBA_WIDTH, GBA_HEIGHT);
}
}