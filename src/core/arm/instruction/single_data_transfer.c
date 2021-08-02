#include "single_data_transfer.h"
#include "log.h"

u32 shift_single_data_transfer(registers_t* regs) {
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

arm_handler arm_handle_single_data_transfer(u32 instruction) {
  if(bit(instruction, 20)) {
    return &arm_ldr;
  } else {
    return &arm_str;
  }
}

ARM_INSTRUCTION(str) {
  u8 rd = bits(registers->instruction, 12, 15);
  u8 rn = bits(registers->instruction, 16, 19);
  u32 address = registers->gpr[rn];
  u32 offset = bit(registers->instruction, 25) ? shift_single_data_transfer(registers) : registers->instruction & 0xfff;
  logdebug("str r%d, [r%d, %08X]\n", rd, rn, offset);
  if(bit(registers->instruction, 24)) {
    address = bit(registers->instruction, 23) ? address + offset : address - offset;
  }

  if(bit(registers->instruction, 22)) {
    logfatal("strb!\n");
  } else {
    write_32(mem, address, registers->gpr[rd]);
  }

  if(bit(registers->instruction, 21)) {
    registers->gpr[rn] = address;
  }
}

ARM_INSTRUCTION(ldr) {
  u8 rd = bits(registers->instruction, 12, 15);
  u8 rn = bits(registers->instruction, 16, 19);
  u32 address = registers->gpr[rn];
  u32 offset = bit(registers->instruction, 25) ? shift_single_data_transfer(registers) : registers->instruction & 0xfff;
  logdebug("ldr r%d, [r%d, %08X]\n", rd, rn, offset);
  if(bit(registers->instruction, 24)) {
    address = bit(registers->instruction, 23) ? address + offset : address - offset;
  }

  if(bit(registers->instruction, 22)) {
    logfatal("ldrb!\n");
  } else {
    registers->gpr[rd] = read_32(mem, address);
    if(rd == 15) {
      flush_pipe_32(registers, mem);
    }
  }

  if(bit(registers->instruction, 21)) {
    registers->gpr[rn] = address;
  }
}

ARM_INSTRUCTION(undefined_single_data_transfer) {
  logfatal("Undefined single data transfer instruction: (%08X) (%s)\n", registers->instruction, binary_str(registers->instruction));
}

ARM_INSTRUCTION(unimplemented_single_data_transfer) {
  logfatal("Unimplemented single data transfer instruction: (%08X) (%s)\n", registers->instruction, binary_str(registers->instruction));
}
