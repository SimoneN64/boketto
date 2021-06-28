#include "cpu.h"

namespace boketto::core
{
Cpu::Cpu(std::string bios) : mem(bios) {
  GenerateARMLut();
  GenerateThumbLut();
}

void Cpu::step() {
  gpr[PC] += 4;
  next_pc += 8;
}

void Cpu::GenerateARMLut() {
  for(int i = 0; i < 4096; i++) {
    arm_lut[i] = &ARMUndefined;
  }
}

void Cpu::GenerateThumbLut() {
  for(int i = 0; i < 1024; i++) {
    thumb_lut[i] = &ThumbUndefined;
  }
}
}