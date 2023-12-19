#include <arm/instruction/data_processing.h>
#include <log.h>
#include <helpers.h>

u32 arm_data_processing_shift(u32 instr, registers_t* regs, bool* carry_out) {
  if(bit(instr, 25)) {
    u32 input = instr & 0xff;
    u8 amount = bits(instr, 8, 11) * 2;
    if(amount != 0) {
      *carry_out = (input >> (amount - 1)) & 1;
    }
    return ror32(input, amount);
  } else {
    u32 input = regs->gpr[instr & 0xF];
    u8 amount = 0;
    u8 type = bits(instr, 5, 6);

    amount = bit(instr, 4)
             ? regs->gpr[bits(instr, 8, 11)] & 0xff
             : bits(instr, 7, 11);

    return shift_reg(type, input, amount, carry_out, regs);
  }
}

arm_handler arm_handle_data_processing(u8 opcode) {
  switch(opcode) {
    case 0b0000: return &arm_and;
    case 0b0001: return &arm_unimplemented_data_processing;
    case 0b0010: return &arm_unimplemented_data_processing;
    case 0b0011: return &arm_unimplemented_data_processing;
    case 0b0100: return &arm_add;
    case 0b0101: return &arm_unimplemented_data_processing;
    case 0b0110: return &arm_unimplemented_data_processing;
    case 0b0111: return &arm_unimplemented_data_processing;
    case 0b1000: return &arm_tst;
    case 0b1001: return &arm_unimplemented_data_processing;
    case 0b1010: return &arm_cmp;
    case 0b1011: return &arm_unimplemented_data_processing;
    case 0b1100: return &arm_orr;
    case 0b1101: return &arm_mov;
    case 0b1110: return &arm_unimplemented_data_processing;
    case 0b1111: return &arm_unimplemented_data_processing;
    default:
      return &arm_undefined_data_processing;
  }
}

ARM_INSTRUCTION(and) {
  bool carry_out = registers->cpsr.carry;
  bool I = bit(instr, 25);
  u8 rn = bits(instr, 16, 19);
  u8 rd = bits(instr, 12, 15);
  u32 op1 = registers->gpr[rn], op2 = arm_data_processing_shift(instr, registers, &carry_out);
  
  u32 result = op1 & op2;
  registers->cpsr.negative = result >> 31;
  registers->cpsr.zero = result == 0;
  registers->cpsr.carry = carry_out;
}

ARM_INSTRUCTION(cmp) {
  bool dummy = false;
  bool I = bit(instr, 25);
  u8 rn = bits(instr, 16, 19);
  u32 op1 = registers->gpr[rn], op2 = arm_data_processing_shift(instr, registers, &dummy);
  
  u32 result = op1 - op2;
  registers->cpsr.negative = result >> 31;
  registers->cpsr.carry = result <= op1;
  registers->cpsr.zero = result == 0;
  registers->cpsr.overflow = ((op1 ^ result) & (~op2 ^ result)) >> 31;
}

ARM_INSTRUCTION(mov) {
  u8 rd = bits(instr, 12, 15);
  bool I = bit(instr, 25);
  bool carry_out = registers->cpsr.carry;
  u32 result = arm_data_processing_shift(instr, registers, &carry_out);
  

  if(rd == PC) {
    if(bit(instr, 20)) {
      registers->cpsr.raw = registers->spsr.raw;
      change_mode(registers, registers->cpsr.mode);
    }
    set_pc(false, mem, registers, result, registers->cpsr.thumb);
  } else {
    registers->gpr[rd] = result;
    if(bit(instr, 20)) {
      registers->cpsr.negative = result >> 31;
      registers->cpsr.zero = result == 0;
      registers->cpsr.carry = carry_out;
    }
  }
}

ARM_INSTRUCTION(orr) {
  u8 rd = bits(instr, 12, 15);
  u8 rn = bits(instr, 16, 19);

  bool I = bit(instr, 25);
  bool carry_out = registers->cpsr.carry;
  u32 result = arm_data_processing_shift(instr, registers, &carry_out);
  
  result |= registers->gpr[rn];

  if(rd == 15) {
    if(bit(instr, 20)) {
      registers->cpsr.raw = registers->spsr.raw;
      change_mode(registers, registers->cpsr.mode);
    }
    set_pc(false, mem, registers, result, registers->cpsr.thumb);
  } else {
    registers->gpr[rd] = result;
    if(bit(instr, 20)) {
      registers->cpsr.negative = result >> 31;
      registers->cpsr.zero = result == 0;
      registers->cpsr.carry = carry_out;
    }
  }
}

ARM_INSTRUCTION(add) {
  u8 rd = bits(instr, 12, 15);
  u8 rn = bits(instr, 16, 19);
  bool dummy = false;
  bool I = bit(instr, 25);
  u32 op1 = arm_data_processing_shift(instr, registers, &dummy);
  u32 op2 = registers->gpr[rn];
  u32 result = op1 + op2;
  

  if(rd == PC) {
    if(bit(instr, 20)) {
      registers->cpsr.raw = registers->spsr.raw;
      change_mode(registers, registers->cpsr.mode);
    }
    set_pc(false, mem, registers, result, registers->cpsr.thumb);
  } else {
    registers->gpr[rd] = result;
    if(bit(instr, 20)) {
      registers->cpsr.negative = result >> 31;
      registers->cpsr.zero = result == 0;
      registers->cpsr.overflow = ((op1 ^ result) & (op2 ^ result)) >> 31;
      registers->cpsr.carry = (UINT32_MAX - op1) < op2;
    }
  }
}

ARM_INSTRUCTION(tst) {
  bool carry_out = registers->cpsr.carry;
  bool I = bit(instr, 25);
  u8 rn = bits(instr, 16, 19);
  u32 op1 = registers->gpr[rn];
  u32 op2 = arm_data_processing_shift(instr, registers, &carry_out);
  
  u32 res = op1 & op2;
  registers->cpsr.negative = res >> 31;
  registers->cpsr.zero = res == 0;
  registers->cpsr.carry = carry_out;
}

ARM_INSTRUCTION(undefined_data_processing) {
  logfatal("Undefined data processing instruction: (%08X) (%s)\n", instr, binary_str(instr, 32));
}

ARM_INSTRUCTION(unimplemented_data_processing) {
  logfatal("Unimplemented data processing instruction: (%08X) (%s)\n", instr, binary_str(instr, 32));
}
