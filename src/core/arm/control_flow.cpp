#include "cpu.h"

namespace boketto::core
{
using namespace boketto::util;
void Cpu::b(u32 instruction) {
  bool link = (instruction >> 24) & 1;
  u32 addr = sign_extend<s32, 24>(instruction) << 2;
  if(link) {
    gpr[LR] = gpr[PC] - 4;
  }
  gpr[PC] += addr;
}

Cpu::ARMHandler Cpu::Branch(u32 instruction) {
  if((instruction & 0xF0000000) == 0xF0000000) {
    printf("Branch and change to thumb unhandled\n");
    exit(1);
  }

  return &b;
}
}