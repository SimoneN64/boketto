#pragma once
#include "cpu.h"

namespace boketto::core
{
struct Core
{
  Core(std::string bios);
  void run();
  void LoadROM(std::string path);
  Cpu cpu;
  bool init = false;
};
}