#pragma once
#include "core.h"

namespace boketto::frontend
{
struct MainWindow
{
  MainWindow(std::string title);
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
  SDL_Texture* texture = nullptr;
};
}