#include <arm/instruction/load_store.h>
#include <log.h>
#include <helpers.h>

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

arm_handler arm_handle_load_store_multiple(u32 instruction) {
  if(bit(instruction, 20)) {
    return &arm_ldm;
  } else {
    return &arm_stm;
  }
}

u32 arm_sdt_shift(registers_t* regs) {
  u8 type = bits(regs->instruction, 5, 6);
  u8 amount = bits(regs->instruction, 7, 11);
  u32 input = regs->gpr[regs->instruction & 0xf];
  bool carry_out = regs->cpsr.carry;
  u32 result = shift_imm(type, input, amount, &carry_out, regs);
  return result;
}

ARM_INSTRUCTION(stm) {
  u8 rn = bits(registers->instruction, 16, 19);
  u32 base_address = registers->gpr[rn];

  u16 list_mask = registers->instruction & 0xffff;
  print_list(false, registers->instruction);
  bool increment = bit(registers->instruction, 23);
  bool before = bit(registers->instruction, 24);

  assert(list_mask != 0);

  u8 count_regs = 0;
  for(u8 i = 0; i < 15; i++) {
    if(bit(list_mask, i)) {
      if(before) {
        base_address += increment ? 4 : -4;
      }
      write_32(mem, base_address, registers->gpr[i]);
      if(!before) {
        base_address += increment ? 4 : -4;
      }

      count_regs++;
    }
  }

  if(W(registers->instruction)) {
    registers->gpr[rn] += U(registers->instruction) ? (count_regs * 4) : -(count_regs * 4);
  }
}

ARM_INSTRUCTION(ldm) {
  u8 rn = bits(registers->instruction, 16, 19);
  u32 base_address = registers->gpr[rn];

  u16 list_mask = registers->instruction & 0xffff;
  print_list(false, registers->instruction);
  bool increment = bit(registers->instruction, 23);
  bool before = bit(registers->instruction, 24);

  assert(list_mask != 0);

  u8 count_regs = 0;
  for(u8 i = 0; i < 15; i++) {
    if(bit(list_mask, i)) {
      if(before) {
        base_address += increment ? 4 : -4;
      }
      registers->gpr[i] = read_32(mem, base_address);
      if(!before) {
        base_address += increment ? 4 : -4;
      }
      count_regs++;
    }
  }

  if(W(registers->instruction)) {
    registers->gpr[rn] += U(registers->instruction) ? (count_regs * 4) : -(count_regs * 4);
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
  u32 offset = I(instr) ? arm_sdt_shift(registers) : instr & 0xfff;

  if(P(instr)) {
    address = U(instr) ? address + offset : address - offset;
  }

  address &= ~3;

  logdebug("str r%d, [r%d, %08X]\n", rd(instr), rn(instr), address);

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
  u32 offset = I(instr) ? arm_sdt_shift(registers) : instr & 0xfff;

  if(P(instr)) {
    address = U(instr) ? address + offset : address - offset;
  }

  if(B(instr)) {
    logdebug("ldrb r%d, [r%d, %08X]\n", rd(instr), rn(instr), address);
    if(rd(instr) != 15) {
      registers->gpr[rd(instr)] = read_8(mem, address);
    } else {
      flush_pipe_32(registers, mem);
    }
  } else {
    address &= ~3;
    logdebug("ldr r%d, [r%d, %08X]\n", rd(instr), rn(instr), address);
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
