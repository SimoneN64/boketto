#include <arm/instruction/status_register.h>
#include <log.h>

ARM_INSTRUCTION(msr) {
  bool privileged = registers->cpsr.mode != 0x10;
  u32 operand = 0;
  operand = bit(instr, 25)
            ? ror32(instr & 0xff, bits(instr, 8, 11))
            : registers->gpr[instr & 0xf];

  u32 mask = (bit(instr, 16) && privileged) * 0x000000ff
           | (bit(instr, 19) * 0xf0000000);

  if(bit(instr, 22)) {
    registers->spsr.raw = (registers->spsr.raw & ~mask) | (operand & mask);
  } else {
    u32 val = (registers->cpsr.raw & ~mask) | (operand & mask);
    change_mode(registers, val & 0x1f);
    registers->cpsr.raw = val;
  }
}

ARM_INSTRUCTION(mrs) {
  u8 rd = (instr >> 12) & 0xf;
  if(bit(instr, 22)) {
    registers->gpr[rd] = registers->spsr.raw;
  } else {
    registers->gpr[rd] = registers->cpsr.raw;
  }
}