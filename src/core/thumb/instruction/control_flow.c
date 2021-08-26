#include "thumb/instruction/control_flow.h"
#include "log.h"
#include "helpers.h"

THUMB_INSTRUCTION(bcond) {
  u8 cond = bits(registers->instruction, 8, 11);
  s32 addr = sign_extend32(registers->instruction, 8) << 1;
  logdebug("b %08X\n", addr);
  if(get_condition(registers->cpsr, cond)) {
    logdebug("Condition passed!\n");
    set_pc(false, mem, registers, registers->gpr[PC] + addr, registers->cpsr.thumb);
  } else {
    logdebug("Condition not passed!\n");
  }
}

THUMB_INSTRUCTION(bx) {
  u8 rm = (registers->instruction >> 3) & 0xf;
  registers->cpsr.thumb = registers->gpr[rm] & 1;
  logdebug("bx r%d\n", rm);
  set_pc(false, mem, registers, registers->gpr[rm], registers->cpsr.thumb);
}

THUMB_INSTRUCTION(bl_high) {
  u16 offset = registers->instruction & 0x7FF;
  logdebug("bl[h == 0b10] %08X\n", offset);
  registers->gpr[LR] = registers->gpr[PC] + (sign_extend32(offset, 11) << 12);
}

THUMB_INSTRUCTION(bl_low) {
  s16 offset = registers->instruction & 0x7FF;
  logdebug("bl[h == 0b11] %08X\n", offset);
  u32 pc = registers->gpr[PC];
  set_pc(false, mem, registers, registers->gpr[LR] + (offset << 1), registers->cpsr.thumb);
  registers->gpr[LR] = (pc - 2) | 1;
}

THUMB_INSTRUCTION(unimplemented_control_flow) {
  logfatal("Unimplemented control flow instruction %04X (%s)\n", registers->instruction, binary_str(registers->instruction, 16));
}