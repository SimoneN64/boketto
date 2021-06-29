#include "cpu.h"

namespace boketto::core
{
Cpu::Cpu(std::string bios) : mem(bios) {
  GenerateARMLut();
  GenerateThumbLut();
  cpsr.thumb = 0;
  gpr[PC] = 0x8000004;
}

void Cpu::step() {
  if(cpsr.thumb) {
    gpr[PC] += 2;
    auto instruction = mem.ReadHalf(gpr[PC] - 4);
    (this->*thumb_lut[instruction >> 6])(instruction);
  } else {
    gpr[PC] += 4;
    auto instruction = mem.ReadWord(gpr[PC] - 8);
    if(GetCondition(instruction >> 28)) {
      (this->*arm_lut[((instruction >> 16) & 0xFF0) | ((instruction >> 4) & 0xF)])(instruction);
    }
  }
}

void Cpu::GenerateARMLut() {
  for(u32 i = 0; i < 4096; i++) {
    u16 mask = ((i & 0xFF0) << 16) | ((i & 0xF) << 4);
    if((mask & 0x0F900000) == 0x01000000) {
      arm_lut[i] = Misc(i);
    } else if ((mask & 0x0E000000) == 0x0A000000) {
      arm_lut[i] = Branch(i);
    }
    arm_lut[i] = &ARMUndefined;
  }
}

void Cpu::GenerateThumbLut() {
  for(u32 i = 0; i < 1024; i++) {
    thumb_lut[i] = &ThumbUndefined;
  }
}
}