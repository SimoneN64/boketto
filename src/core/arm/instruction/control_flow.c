#include "control_flow.h"
#include "log.h"

ARM_INSTRUCTION(branch) {
  bool link = (registers->instruction >> 24) & 1;
  s32 addr = sex((s32)registers->instruction, 24) << 2;
  
  logdebug("b%s %d\n", link ? "l" : "", addr);
  
  if(link) {
    registers->gpr[LR] = registers->gpr[PC] - 4;
  }

  registers->gpr[PC] += addr;
  flush_pipe_32(registers, mem);
}