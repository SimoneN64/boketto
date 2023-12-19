#include "thumb/instruction/control_flow.h"
#include "log.h"
#include "helpers.h"

THUMB_INSTRUCTION(bcond) {
  u8 cond = bits(instr, 8, 11);
  s32 addr = sign_extend32(instr, 8) << 1;
  
  if(get_condition(registers->cpsr, cond)) {
    set_pc(false, mem, registers, registers->gpr[PC] + addr, registers->cpsr.thumb);
  } else {
    
  }
}

THUMB_INSTRUCTION(bx) {
  u8 rm = (instr >> 3) & 0xf;
  registers->cpsr.thumb = registers->gpr[rm] & 1;
  
  set_pc(false, mem, registers, registers->gpr[rm], registers->cpsr.thumb);
}

THUMB_INSTRUCTION(bl_high) {
  u16 offset = instr & 0x7FF;
  
  registers->gpr[LR] = registers->gpr[PC] + (sign_extend32(offset, 11) << 12);
}

THUMB_INSTRUCTION(bl_low) {
  s16 offset = instr & 0x7FF;
  
  u32 pc = registers->gpr[PC];
  set_pc(false, mem, registers, registers->gpr[LR] + (offset << 1), registers->cpsr.thumb);
  registers->gpr[LR] = (pc - 2) | 1;
}

THUMB_INSTRUCTION(unimplemented_control_flow) {
  logfatal("Unimplemented control flow instruction %04X (%s)\n", instr, binary_str(instr, 16));
}