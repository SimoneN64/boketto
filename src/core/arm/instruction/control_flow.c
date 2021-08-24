#include <arm/instruction/control_flow.h>
#include <log.h>
#include <helpers.h>

ARM_INSTRUCTION(b) {
  bool link = bit(registers->instruction, 24);
  s32 addr = sign_extend32((s32)registers->instruction & 0xFFFFFF, 24) << 2;
  set_pc(link, mem, registers, registers->gpr[PC] + addr);
  
  logdebug("b%s %08X\n", link ? "l" : "", addr);
}

ARM_INSTRUCTION(bx) {
  u8 rm = registers->instruction & 0xf;
  registers->cpsr.thumb = registers->gpr[rm] & 1;
  set_pc(false, mem, registers, registers->gpr[rm]);

  logdebug("bx %08X\n", registers->gpr[rm]);
}