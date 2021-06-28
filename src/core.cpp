#include "core.h"

namespace boketto::core
{
Core::Core(std::string bios) : cpu(bios) {}

void Core::run() {
  if(init) {
    for(int i = 0; i < 279620; i++) {
      cpu.step();
    }
  }
}

void Core::LoadROM(std::string path) {
  cpu.mem.LoadROM(path);
  init = true;
}
}