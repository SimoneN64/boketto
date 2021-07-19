#include "control_flow.h"

void branch(registers_t* registers) {
  bool link = (registers->instruction >> 24) & 1;
  s32 addr = sex((s32)registers->instruction, 24) << 2;
  
  log("b%s %d\n", link ? "l" : "", addr);
  
  if(link) {
    registers->gpr[LR] = registers->gpr[PC] - 4;
  }

  registers->gpr[PC] += addr;
}