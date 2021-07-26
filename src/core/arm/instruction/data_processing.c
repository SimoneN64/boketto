#include "data_processing.h"
#include "log.h"

arm_handler arm_handle_data_processing(u32 instruction) {
  switch(bits(instruction, 21, 24)) {
    case 0b0000: return &arm_unimplemented_data_processing;
    case 0b0001: return &arm_unimplemented_data_processing;
    case 0b0010: return &arm_unimplemented_data_processing;
    case 0b0011: return &arm_unimplemented_data_processing;
    case 0b0100: return &arm_unimplemented_data_processing;
    case 0b0101: return &arm_unimplemented_data_processing;
    case 0b0110: return &arm_unimplemented_data_processing;
    case 0b0111: return &arm_unimplemented_data_processing;
    case 0b1000: return &arm_unimplemented_data_processing;
    case 0b1001: return &arm_unimplemented_data_processing;
    case 0b1010: return &arm_unimplemented_data_processing;
    case 0b1011: return &arm_unimplemented_data_processing;
    case 0b1100: return &arm_unimplemented_data_processing;
    case 0b1101: return &arm_mov;
    case 0b1110: return &arm_unimplemented_data_processing;
    case 0b1111: return &arm_unimplemented_data_processing;
    default: return &arm_undefined_data_processing;
  }
}

u32 shift_data_processing(registers_t* regs) {
  u32 result = 0;
  if(bit(regs->instruction, 25)) {
    result = ror32(regs->instruction & 0xff, bits(regs->instruction, 8, 11));
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

ARM_INSTRUCTION(mov) {
  u8 rd = bits(registers->instruction, 12, 15);
  logdebug("mov r%d, %08X\n", rd, registers->instruction & 0xfff);
  if(rd == 15) {
    flush_pipe_32(registers, mem);
  }
  registers->gpr[rd] = shift_data_processing(registers);
  registers->cpsr.negative = bit(registers->gpr[rd], 31);
  registers->cpsr.zero = registers->gpr[rd] == 0 ? 1 : 0;
}

ARM_INSTRUCTION(undefined_data_processing) {
  logfatal("Undefined data processing instruction: (%08X) (%s)\n", registers->instruction, binary_str(registers->instruction));
}

ARM_INSTRUCTION(unimplemented_data_processing) {
  logfatal("Unimplemented data processing instruction: (%08X) (%s)\n", registers->instruction, binary_str(registers->instruction));
}
