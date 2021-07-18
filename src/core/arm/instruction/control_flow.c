#include "control_flow.h"

void branch(registers_t* registers) {
  u32 instruction = registers->instruction;
  bool link = (instruction >> 24) & 1;
  u32 addr = sex((s32)instruction, 24) << 2;
  
  printf("b%s %08X\n", link ? "l" : "", addr);
  
  if(link) {
    registers->gpr[LR] = registers->gpr[PC] - 4;
  }

  registers->gpr[PC] += addr;
}