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
  u8 rm = registers->instruction & 0xf;

  bool thumb = registers->cpsr.thumb = registers->gpr[rm] & 1;
  u32 addr;
  if(thumb) {
    addr = registers->gpr[rm] & ~1;
  } else {
    addr = registers->gpr[rm] & ~3;
  }
  logdebug("b%sx %08X\n", link ? "l" : "", addr);

  set_pc(link, mem, registers, addr);
}