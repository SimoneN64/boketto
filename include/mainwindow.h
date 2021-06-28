#pragma once
#include "core.h"
#include <memory>

namespace boketto::frontend
{
using namespace boketto::core;
struct MainWindow
{
  ~MainWindow();
  MainWindow(std::string title);
  void Run();
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
  SDL_Texture* texture = nullptr;
  std::unique_ptr<Core> core;
};
}