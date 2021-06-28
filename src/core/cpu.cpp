#include "cpu.h"

namespace boketto::core
{
Cpu::Cpu(std::string bios) : mem(bios) {
  GenerateARMLut();
  GenerateThumbLut();
}

void Cpu::step() {
  if(cpsr.thumb) {
    gpr[PC] &= ~1;
    auto instruction = mem.ReadHalf(gpr[PC]);
    gpr[PC] += 2;
    next_pc += 4;
    (this->*thumb_lut[instruction >> 6])(instruction);
  } else {
    gpr[PC] &= ~3;
    auto instruction = mem.ReadWord(gpr[PC]);
    gpr[PC] += 4;
    next_pc += 8;
    if(GetCondition(instruction >> 28)) {
      (this->*arm_lut[((instruction >> 16) & 0xFF0) | ((instruction >> 4) & 0xF)])(instruction);
    } else {
      gpr[PC] += 4;
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