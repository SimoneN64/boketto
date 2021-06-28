#include "mem.h"

namespace boketto::core
{
Mem::Mem(std::string bios) {
  LoadBIOS(bios);
  rom = new u8[ROM_SIZE];
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

auto Mem::ReadByte(u32 addr) -> u8 {
  switch (addr)
  {
  case 0 ... 0x3FFF:
    return bios[addr];
  case 0x4000 ... 0x1FFFFFF:
    return 0xff;
  case 0x2000000 ... 0x203FFFF:
    return iwram[addr & 0x3FFFF];
  case 0x2040000 ... 0x2FFFFFF:
    return 0xff;
  case 0x3000000 ... 0x3007FFF:
    return ewram[addr & 0x7FFF];
  case 0x8000000 ... 0x9FFFFFF:
    return rom[addr & 0x1FFFFFF];
  default:
    printf("Unhandled read at %08X\n", addr);
    break;
  }
}

auto Mem::ReadHalf(u32 addr) -> u16 {
  return (ReadByte(addr + 1) << 8) | ReadByte(addr);
}

auto Mem::ReadWord(u32 addr) -> u32 {
  return ((u32)ReadHalf(addr + 2) << 16) | ReadHalf(addr);
}

}