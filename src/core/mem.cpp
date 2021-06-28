#include "mem.h"

namespace boketto::core
{
Mem::Mem(std::string bios) {
  LoadBIOS(bios);
  rom = new u8[0xA000000];
}

void Mem::LoadBIOS(std::string path) {
  std::ifstream file{path, std::ios::binary};
  file.unsetf(std::ios::skipws);

  if(!file.is_open()) {
    printf("Couldn't find bios \"%s\"\n", path.c_str());
    exit(1);
  }

  file.read((char*)bios.data(), BIOS_SIZE);
  file.close();
}

void Mem::LoadROM(std::string path) {
  std::ifstream file{path, std::ios::binary};
  file.unsetf(std::ios::skipws);

  if(!file.is_open()) {
    printf("Couldn't find rom \"%s\"\n", path.c_str());
    exit(1);
  }

  file.read((char*)rom, ROM_SIZE);
  file.close();
}
}