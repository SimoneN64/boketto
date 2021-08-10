#include "arm/instruction/control_flow.h"
#include "log.h"
#include "helpers.h"

ARM_INSTRUCTION(b) {
  bool link = bit(registers->instruction, 24);
  s32 addr = sign_extend32(registers->instruction & 0xFFFFFF, 24) << 2;
  
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