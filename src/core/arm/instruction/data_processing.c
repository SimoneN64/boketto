#include "data_processing.h"
#include "log.h"

u32 shift_data_processing(registers_t* regs) {
  u32 result = 0;
  if(bit(regs->instruction, 25)) {
    result = ror32(regs->instruction & 0xff, bits(regs->instruction, 8, 11) * 2);
  } else {
    result = regs->gpr[regs->instruction & 0xf];
    u32 amount = bit(regs->instruction, 4) ? regs->gpr[bits(regs->instruction, 8, 11)] : bits(regs->instruction, 7, 11);
    switch(bits(regs->instruction, 5, 6)) {
      case 0b00:
        regs->cpsr.carry = bit(regs->gpr[regs->instruction & 0xf], 32 - amount);
        result = regs->gpr[regs->instruction & 0xf] << amount;
        break;
      case 0b01:
        regs->cpsr.carry = bit(regs->gpr[regs->instruction & 0xf], amount - 1);
        result = regs->gpr[regs->instruction & 0xf] >> amount;
        break;
      case 0b10:
        regs->cpsr.carry = bit(regs->gpr[regs->instruction & 0xf], amount - 1);
        result = (s32)(regs->gpr[regs->instruction & 0xf]) >> amount;
        break;
      case 0b11:
        regs->cpsr.carry = bit(regs->gpr[regs->instruction & 0xf], amount - 1);
        result = ror32(regs->gpr[regs->instruction & 0xf], amount);
        break;
    }
  }

  return result;
}

arm_handler arm_handle_data_processing(u32 instruction) {
  switch(bits(instruction, 21, 24)) {
    case 0b0000: return &arm_unimplemented_data_processing;
    case 0b0001: return &arm_unimplemented_data_processing;
    case 0b0010: return &arm_unimplemented_data_processing;
    case 0b0011: return &arm_unimplemented_data_processing;
    case 0b0100: return &arm_add;
    case 0b0101: return &arm_unimplemented_data_processing;
    case 0b0110: return &arm_unimplemented_data_processing;
    case 0b0111: return &arm_unimplemented_data_processing;
    case 0b1000: return &arm_unimplemented_data_processing;
    case 0b1001: return &arm_unimplemented_data_processing;
    case 0b1010: return &arm_cmp;
    case 0b1011: return &arm_unimplemented_data_processing;
    case 0b1100: return &arm_unimplemented_data_processing;
    case 0b1101: return &arm_mov;
    case 0b1110: return &arm_unimplemented_data_processing;
    case 0b1111: return &arm_unimplemented_data_processing;
    default: return &arm_undefined_data_processing;
  }
}

ARM_INSTRUCTION(cmp) {
  u8 rn = bits(registers->instruction, 16, 19);
  u32 op1 = registers->gpr[rn], op2 = shift_data_processing(registers);
  u32 result = op1 - op2;
  registers->cpsr.negative = bit(result, 31);
  registers->cpsr.carry = result <= op1;
  registers->cpsr.zero = result == 0;
  registers->cpsr.overflow = ((op1 ^ result) & (~op2 ^ result)) >> 31;
}

ARM_INSTRUCTION(mov) {
  u8 rd = bits(registers->instruction, 12, 15);
  logdebug("mov r%d, %08X\n", rd, shift_data_processing(registers));
  registers->gpr[rd] = shift_data_processing(registers);

  if(bit(registers->instruction, 20)) {
    registers->cpsr.negative = registers->gpr[rd] >> 31;
    registers->cpsr.zero = registers->gpr[rd] == 0;
    if(rd == 15) {
      registers->cpsr.raw = registers->spsr.raw;
      flush_pipe_32(registers, mem);
    }
  }
}

ARM_INSTRUCTION(add) {
  u8 rd = bits(registers->instruction, 12, 15);
  u8 rn = bits(registers->instruction, 16, 19);
  logdebug("add r%d, r%d, %08X\n", rd, rn, shift_data_processing(registers));
  u32 op1 = shift_data_processing(registers);
  u32 op2 = registers->gpr[rn];
  u64 result = op1 + op2;
  registers->gpr[rd] = result;

  if(bit(registers->instruction, 20)) {
    registers->cpsr.negative = result >> 31;
    registers->cpsr.zero = result == 0;
    registers->cpsr.overflow = ((op1 ^ result) & (op2 ^ result)) >> 31;
    registers->cpsr.carry = result >> 32;
    if(rd == 15) {
      registers->cpsr.raw = registers->spsr.raw;
      flush_pipe_32(registers, mem);
    }
  }
}

ARM_INSTRUCTION(undefined_data_processing) {
  logfatal("Undefined data processing instruction: (%08X) (%s)\n", registers->instruction, binary_str(registers->instruction));
}

ARM_INSTRUCTION(unimplemented_data_processing) {
  logfatal("Unimplemented data processing instruction: (%08X) (%s)\n", registers->instruction, binary_str(registers->instruction));
}
