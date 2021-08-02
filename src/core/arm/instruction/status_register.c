#include "status_register.h"
#include "log.h"

arm_handler arm_handle_status_register(u32 instruction) {
  if(bit(instruction, 21)) {
    return &arm_msr;
  } else {
    return &arm_mrs;
  }
}

ARM_INSTRUCTION(msr) {
  u32 operand = 0;
  char operand_str[8];
  if(bit(registers->instruction, 25)) {
    operand = ror32(registers->instruction & 0xff, bits(registers->instruction, 8, 11));
    snprintf(operand_str, 8, "%08X", operand);
  } else {
    operand = registers->gpr[registers->instruction & 0xf];
    snprintf(operand_str, 8, "r%d", registers->instruction & 0xf);
  }

  u32 byte_mask = (bit(registers->instruction, 16) * 0x000000FF) |
                  (bit(registers->instruction, 17) * 0x0000FF00) |
                  (bit(registers->instruction, 18) * 0x00FF0000) |
                  (bit(registers->instruction, 19) * 0xFF000000);
  if(bit(registers->instruction, 22)) {
    u32 mask = byte_mask & (0xF0000000 | 0x0000000F | 0x00000020);
    registers->spsr.raw = (registers->spsr.raw & ~mask) | (operand & mask);
    logdebug("msr spsr, %s\n", operand_str);
  } else {
    u32 mask = byte_mask & 0xF0000000;
    registers->cpsr.raw = (registers->cpsr.raw & ~mask) | (operand & mask);
    logdebug("msr cpsr, %s\n", operand_str);
  }
}

ARM_INSTRUCTION(mrs) {
  logfatal("Unimplemented MRS\n");
}