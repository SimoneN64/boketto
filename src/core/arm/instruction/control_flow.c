#include "control_flow.h"
#include "log.h"

ARM_INSTRUCTION(b) {
  bool link = bit(registers->instruction, 24);
  s32 addr = sex(registers->instruction & 0xFFFFFF, 24) << 2;
  
  logdebug("b%s %08X\n", link ? "l" : "", addr);

  set_pc(link, mem, registers, registers->gpr[PC] + addr);
}

ARM_INSTRUCTION(bx) {
  bool link = bit(registers->instruction, 5);
  u32 addr = registers->gpr[registers->instruction & 0xf];
  registers->cpsr.thumb = addr & 1;
  logdebug("b%sx %08X\n", link ? "l" : "", addr);

  set_pc(link, mem, registers, addr);
}

void set_pc(bool link, mem_t* mem, registers_t* registers, u32 value) {
  if(link) {
    registers->gpr[LR] = registers->gpr[PC] - 4;
  }

  registers->gpr[PC] = value;
  flush_pipe_32(registers, mem);
}