#pragma once
#include "core.h"
#include <memory>
#include "ini.h"
#include "nfd.h"

namespace boketto::frontend
{
using namespace boketto::core;
struct MainWindow
{
  ~MainWindow();
  MainWindow(std::string title);
  void Run();
  void OpenFile();
  mINI::INIFile file;
  mINI::INIStructure ini;
  SDL_Window* window = nullptr;
  SDL_Renderer* renderer = nullptr;
  SDL_Texture* texture = nullptr;
  std::unique_ptr<Core> core;
};
}