#include "arm/instruction/single_data_transfer.h"
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
  if(bit(instruction, 26)) {
    if (L(instruction)) {
      return &arm_ldr;
    } else {
      return &arm_str;
    }
  } else {
    if (L(instruction)) {
      return &arm_ldrh;
    } else {
      return &arm_strh;
    }
  }
}

ARM_INSTRUCTION(strh) {
  u32 instr = registers->instruction;
  bool I = B(instr); // STRH/LDRH's 'I' is STR/LDR's 'B'
  u32 address = registers->gpr[rn(instr)];
  u32 offset = I ? (bits(instr, 8, 11) << 4) | (instr & 0xf) : registers->gpr[rm(instr)];

  logdebug("strh r%d, [r%d, %08X]\n", rd(instr), rn(instr), offset);

  if(P(instr)) {
    address = U(instr) ? address + offset : address - offset;
  }

  address &= ~1;

  write_16(mem, address, registers->gpr[rd(instr)]);

  if(W(instr) || !P(instr)) {
    if(!P(instr)) {
      address = U(instr) ? address + offset : address - offset;
    }

    if(rn(instr) != 15) {
      registers->gpr[rn(instr)] = address;
    } else {
      flush_pipe_32(registers, mem);
    }
  }
}

ARM_INSTRUCTION(ldrh) {
  u32 instr = registers->instruction;
  bool I = B(instr); // STRH/LDRH's 'I' is STR/LDR's 'B'
  u32 address = registers->gpr[rn(instr)];
  u32 offset = I ? (bits(instr, 8, 11) << 4) | (instr & 0xf) : registers->gpr[rm(instr)];

  address &= ~1;

  logdebug("ldrh r%d, [r%d, %08X]\n", rd(instr), rn(instr), offset);

  if(P(instr)) {
    address = U(instr) ? address + offset : address - offset;
  }

  if(rd(instr) != 15) {
    registers->gpr[rd(instr)] = read_16(mem, address);
  } else {
    flush_pipe_32(registers, mem);
  }

  if(W(instr) || !P(instr)) {
    if(!P(instr)) {
      address = U(instr) ? address + offset : address - offset;
    }

    if(rn(instr) != 15) {
      registers->gpr[rn(instr)] = address;
    } else {
      flush_pipe_32(registers, mem);
    }
  }
}

ARM_INSTRUCTION(str) {
  u32 instr = registers->instruction;
  u32 address = registers->gpr[rn(instr)];
  u32 offset = I(instr) ? shift_single_data_transfer(registers) : instr & 0xfff;

  logdebug("str r%d, [r%d, %08X]\n", rd(instr), rn(instr), offset);

  if(P(instr)) {
    address = U(instr) ? address + offset : address - offset;
  }

  address &= ~3;

  if(B(instr)) {
    logfatal("strb!\n");
  } else {
    write_32(mem, address, registers->gpr[rd(instr)]);
  }

  if(W(instr) || !P(instr)) {
    if(!P(instr)) {
      address = U(instr) ? address + offset : address - offset;
    }

    if(rn(instr) != 15) {
      registers->gpr[rn(instr)] = address;
    } else {
      flush_pipe_32(registers, mem);
    }
  }
}

ARM_INSTRUCTION(ldr) {
  u32 instr = registers->instruction;
  u32 address = registers->gpr[rn(instr)];
  u32 offset = I(instr) ? shift_single_data_transfer(registers) : instr & 0xfff;

  logdebug("ldr r%d, [r%d, %08X]\n", rd(instr), rn(instr), offset);

  if(P(instr)) {
    address = U(instr) ? address + offset : address - offset;
  }

  address &= ~3;

  if(B(instr)) {
    logfatal("ldrb!\n");
  } else {
    if(rd(instr) != 15) {
      registers->gpr[rd(instr)] = read_32(mem, address);
    } else {
      flush_pipe_32(registers, mem);
    }
  }

  if(W(instr) || !P(instr)) {
    if(!P(instr)) {
      address = U(instr) ? address + offset : address - offset;
    }

    if(rn(instr) != 15) {
      registers->gpr[rn(instr)] = address;
    } else {
      flush_pipe_32(registers, mem);
    }
  }
}

ARM_INSTRUCTION(undefined_single_data_transfer) {
  logfatal("Undefined single data transfer instruction: (%08X) (%s)\n", registers->instruction, binary_str(registers->instruction, 32));
}

ARM_INSTRUCTION(unimplemented_single_data_transfer) {
  logfatal("Unimplemented single data transfer instruction: (%08X) (%s)\n", registers->instruction, binary_str(registers->instruction, 32));
}
