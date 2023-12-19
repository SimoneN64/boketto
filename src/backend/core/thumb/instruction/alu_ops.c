#include <thumb/instruction/alu_ops.h>
#include <log.h>

THUMB_INSTRUCTION(unimplemented_alu) {
  logfatal("Unimplemented alu opcode %02X\n", bits(instr, 6, 9));
}

THUMB_INSTRUCTION(handle_alu) {
  switch (bits(instr, 6, 9)) {
  case 0x6: thumb_sbc(instr, registers, mem); break;
  case 0xE: thumb_bic(instr, registers, mem); break;
  default: thumb_unimplemented_alu(instr, registers, mem);
  }
}

THUMB_INSTRUCTION(bic) {
  u8 rd = instr & 7;
  u8 rm = (instr >> 3) & 7;
  
  registers->gpr[rd] &= ~registers->gpr[rm];
  registers->cpsr.negative = registers->gpr[rd] >> 31;
  registers->cpsr.zero = registers->gpr[rd] == 0;
}

THUMB_INSTRUCTION(sub_reg) {
  u8 rd = instr & 7;
  u8 rn = (instr >> 3) & 7;
  u8 rm = (instr >> 6) & 7;
  u32 op1 = registers->gpr[rn], op2 = registers->gpr[rm];
  u32 result = op1 - op2;
  
  registers->cpsr.negative = result >> 31;
  registers->cpsr.carry = op2 <= op1;
  registers->cpsr.zero = result == 0;
  registers->cpsr.overflow = ((op1 ^ op2) & (op1 ^ result)) >> 31;
  registers->gpr[rd] = result;
}

THUMB_INSTRUCTION(sub_imm) {
  u8 rd = bits(instr, 8, 10);
  u32 op1 = registers->gpr[rd];
  u8 op2 = instr & 0xff;
  u32 result = op1 - op2;
  
  registers->cpsr.negative = result >> 31;
  registers->cpsr.carry = op2 <= op1;
  registers->cpsr.zero = result == 0;
  registers->cpsr.overflow = ((op1 ^ op2) & (op1 ^ result)) >> 31;
  registers->gpr[rd] = result;
}

THUMB_INSTRUCTION(cmp_imm) {
  u8 rd = bits(instr, 8, 10);
  u32 op1 = registers->gpr[rd];
  u8 op2 = instr & 0xff;
  u32 result = op1 - op2;
  
  registers->cpsr.negative = result >> 31;
  registers->cpsr.carry = op2 <= op1;
  registers->cpsr.zero = result == 0;
  registers->cpsr.overflow = ((op1 ^ op2) & (op1 ^ result)) >> 31;
}

THUMB_INSTRUCTION(lsl_imm) {
  u8 imm = bits(instr, 6, 10);
  u8 rd = instr & 7;
  u8 rm = (instr >> 3) & 7;
  

  if(imm == 0) {
    registers->gpr[rd] = registers->gpr[rm];
  } else {
    registers->cpsr.carry = (registers->gpr[rm] >> (32 - imm)) & 1;
    registers->gpr[rd] = registers->gpr[rm] << imm;
  }

  registers->cpsr.negative = registers->gpr[rd] >> 31;
  registers->cpsr.zero = registers->gpr[rd] == 0;
}

THUMB_INSTRUCTION(lsr_imm) {
  u8 imm = bits(instr, 6, 10);
  u8 rd = instr & 7;
  u8 rm = (instr >> 3) & 7;
  

  if(imm == 0) {
    registers->cpsr.carry = registers->gpr[rm] >> 31;
    registers->gpr[rd] = 0;
  } else {
    registers->cpsr.carry = (registers->gpr[rm] >> (imm - 1)) & 1;
    registers->gpr[rd] = registers->gpr[rm] >> imm;
  }

  registers->cpsr.negative = registers->gpr[rd] >> 31;
  registers->cpsr.zero = registers->gpr[rd] == 0;
}

THUMB_INSTRUCTION(add_reg) {
  u8 rd = instr & 7;
  u8 rm = (instr >> 3) & 7;
  u8 rn = (instr >> 6) & 7;
  u32 op1 = registers->gpr[rn], op2 = registers->gpr[rm];
  
  u64 result = op1 + op2;
  registers->gpr[rd] = result;
  registers->cpsr.negative = (u32)result >> 31;
  registers->cpsr.zero = (u32)result == 0;
  registers->cpsr.carry = result >> 32;
  registers->cpsr.overflow = (~(op1 ^ op2) & (op1 ^ (u32)result)) >> 31;
}

THUMB_INSTRUCTION(add_imm) {
  u8 rd = instr & 7;
  u8 rn = (instr >> 6) & 7;
  u32 op1 = registers->gpr[rn], op2 = instr & 0xff;
  
  u64 result = op1 + op2;
  registers->gpr[rd] = result;
  registers->cpsr.negative = (u32)result >> 31;
  registers->cpsr.zero = (u32)result == 0;
  registers->cpsr.carry = result >> 32;
  registers->cpsr.overflow = (~(op1 ^ op2) & (op1 ^ (u32)result)) >> 31;
}

THUMB_INSTRUCTION(mov_imm) {
  u8 rd = (instr >> 8) & 7;
  u8 imm = instr & 0xff;
  
  registers->gpr[rd] = imm;
  registers->cpsr.negative = 0;
  registers->cpsr.zero = imm == 0;
}

THUMB_INSTRUCTION(mov_high) {
  u8 rd = ((instr >> 4) & 8) | (instr & 7);
  u8 rm = (instr >> 3) & 0xf;
  
  registers->gpr[rd] = registers->gpr[rm];
}

THUMB_INSTRUCTION(sbc) {
  u8 rd = instr & 7;
  u8 rm = (instr >> 3) & 7;
  
  u8 op1 = registers->gpr[rd];
  registers->gpr[rd] -= registers->gpr[rm] - !registers->cpsr.carry;
  registers->cpsr.negative = registers->gpr[rd] >> 31;
  registers->cpsr.zero = registers->gpr[rd] == 0;
  registers->cpsr.overflow = ((op1 ^ registers->gpr[rm]) & (op1 ^ registers->gpr[rd])) >> 31;
  registers->cpsr.carry = registers->gpr[rm] <= op1;
}