#pragma once
#include "common.h"
#include <array>
#include <fstream>
#define BIOS_SIZE 0x4000
#define IWRAM_SIZE 0x40000
#define EWRAM_SIZE 0x8000
#define VRAM_SIZE 0x18000
#define OAM_SIZE 0x400
#define ROM_SIZE 0xA000000

namespace boketto::core
{
class Mem {
public:
  Mem(std::string bios);
  void LoadROM(std::string path);
private:
  void LoadBIOS(std::string path);
  std::array<u8, BIOS_SIZE> bios;
  std::array<u8, IWRAM_SIZE> iwram;
  std::array<u8, EWRAM_SIZE> ewram;
  std::array<u8, VRAM_SIZE> vram;
  std::array<u8, OAM_SIZE> oam;
  std::array<u8, OAM_SIZE> pram;
  u8* rom = nullptr;
};
}