#include "thumb/instruction/alu_ops.h"
#include "log.h"

THUMB_INSTRUCTION(bic) {
  u8 rd = registers->instruction & 7;
  u8 rm = (registers->instruction >> 3) & 7;
  logdebug("bic r%d, r%d\n", rd, rm);
  registers->gpr[rd] &= ~registers->gpr[rm];
  registers->cpsr.negative = registers->gpr[rd] >> 31;
  registers->cpsr.zero = registers->gpr[rd] == 0;
}

THUMB_INSTRUCTION(sub_reg) {
  u8 rd = registers->instruction & 7;
  u8 rn = (registers->instruction >> 3) & 7;
  u8 rm = (registers->instruction >> 6) & 7;
  u32 op1 = registers->gpr[rn], op2 = registers->gpr[rm];
  u32 result = op1 - op2;
  logdebug("sub r%d, r%d, r%d\n", rd, rn, rm);
  registers->cpsr.negative = result >> 31;
  registers->cpsr.carry = result <= op1;
  registers->cpsr.zero = result == 0;
  registers->cpsr.overflow = ((op1 ^ result) & (~op2 ^ result)) >> 31;
  registers->gpr[rd] = result;
}

THUMB_INSTRUCTION(sub_imm) {
  u8 rd = registers->instruction & 7;
  u32 op1 = registers->gpr[rd];
  u8 op2 = registers->instruction & 0xff;
  u32 result = op1 - op2;
  logdebug("sub r%d, %02X\n", rd, op2);
  registers->cpsr.negative = result >> 31;
  registers->cpsr.carry = result <= op1;
  registers->cpsr.zero = result == 0;
  registers->cpsr.overflow = ((op1 ^ result) & (~op2 ^ result)) >> 31;
  registers->gpr[rd] = result;
}

THUMB_INSTRUCTION(lsl) {
  u8 imm = bits(registers->instruction, 6, 10);
  u8 rd = registers->instruction & 7;
  u8 rm = (registers->instruction >> 3) & 7;

  if(imm == 0) {
    registers->gpr[rd] = registers->gpr[rm];
  } else {
    registers->cpsr.carry = (registers->gpr[rm] >> (32 - imm)) & 1;
    registers->gpr[rd] = registers->gpr[rm] >> imm;
  }

  registers->cpsr.negative = registers->gpr[rd] >> 31;
  registers->cpsr.zero = registers->gpr[rd] == 0;

  logdebug("lsl r%d, r%d, %02X\n", rd, rm, imm);
}

THUMB_INSTRUCTION(add_reg) {
  u8 rd = registers->instruction & 7;
  u8 rm = (registers->instruction >> 3) & 7;
  u8 rn = (registers->instruction >> 6) & 7;
  logdebug("add r%d, r%d, r%d\n", rd, rn, rm);
  u64 result = registers->gpr[rm] + registers->gpr[rn];
  registers->gpr[rd] = result;
  registers->cpsr.negative = result >> 31;
  registers->cpsr.zero = result == 0;
  registers->cpsr.carry = result >> 32;
  registers->cpsr.overflow = ((registers->gpr[rn] ^ result) & (registers->gpr[rm] ^ result)) >> 31;
  if(rd == PC) {
    registers->cpsr.raw = registers->spsr.raw;
    flush_pipe_16(registers, mem);
  }
}

THUMB_INSTRUCTION(mov_imm) {
  u8 rd = (registers->instruction >> 8) & 7;
  u8 imm = registers->instruction & 0xff;
  logdebug("mov r%d, %02X\n", rd, imm);
  registers->gpr[rd] = imm;
  registers->cpsr.negative = (registers->gpr[rd] >> 31) & 1;
  registers->cpsr.zero = registers->gpr[rd] == 0;
}