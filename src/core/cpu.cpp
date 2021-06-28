#include "cpu.h"

namespace boketto::core
{
Cpu::Cpu(std::string bios) : mem(bios) {
  GenerateARMLut();
  GenerateThumbLut();
  cpsr.thumb = 0;
  gpr[PC] = 0x8000000;
  gpr[PC] = 0x8000004;
}

void Cpu::step() {
  if(cpsr.thumb) {
    auto instruction = mem.ReadHalf(gpr[PC]);
    gpr[PC] = next_pc;
    next_pc += 2;
    (this->*thumb_lut[instruction >> 6])(instruction);
  } else {
    auto instruction = mem.ReadWord(gpr[PC]);
    gpr[PC] = next_pc;
    next_pc += 4;
    if(GetCondition(instruction >> 28)) {
      (this->*arm_lut[((instruction >> 16) & 0xFF0) | ((instruction >> 4) & 0xF)])(instruction);
    }
  }
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