#include <arm/instruction/control_flow.h>
#include <log.h>
#include <helpers.h>

ARM_INSTRUCTION(b) {
  bool link = bit(instr, 24);
  s32 addr = sign_extend32((s32)instr & 0xFFFFFF, 24) << 2;
  set_pc(link, mem, registers, registers->gpr[PC] + addr, registers->cpsr.thumb); 
}

ARM_INSTRUCTION(bx) {
  u8 rm = instr & 0xf;
  registers->cpsr.thumb = registers->gpr[rm] & 1;
  set_pc(false, mem, registers, registers->gpr[rm], registers->cpsr.thumb);
}