#include <arm/instruction/status_register.h>
#include <log.h>

arm_handler arm_handle_status_register(u32 instruction) {
  return bit(instruction, 21) ? &arm_msr : &arm_mrs;
}

ARM_INSTRUCTION(msr) {
  bool privileged = registers->cpsr.mode != 0x10;
  u32 operand = 0;
  char operand_str[8];
  operand = bit(registers->instruction, 25)
            ? ror32(registers->instruction & 0xff, bits(registers->instruction, 8, 11))
            : registers->gpr[registers->instruction & 0xf];

  u32 mask = (bit(registers->instruction, 16) && privileged) * 0x000000ff
           | (bit(registers->instruction, 19) * 0xf0000000);

  if(bit(registers->instruction, 22)) {
    registers->spsr.raw = (registers->spsr.raw & ~mask) | (operand & mask);
  } else {
    u32 val = (registers->cpsr.raw & ~mask) | (operand & mask);
    change_mode(registers, val & 0x1f);
    registers->cpsr.raw = val;
  }
}

ARM_INSTRUCTION(mrs) {
  u8 rd = (registers->instruction >> 12) & 0xf;
  if(bit(registers->instruction, 22)) {
    registers->gpr[rd] = registers->spsr.raw;
  } else {
    registers->gpr[rd] = registers->cpsr.raw;
  }
}