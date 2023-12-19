#include <arm/instruction/load_store.h>
#include <log.h>
#include <helpers.h>

arm_handler arm_handle_single_data_transfer(u32 instruction) {
  if(bit(instruction, 10)) {
    if (bit(instruction, 4)) {
      return &arm_ldr;
    } else {
      return &arm_str;
    }
  } else {
    if (bit(instruction, 4)) {
      return &arm_ldrh;
    } else {
      return &arm_strh;
    }
  }
}

arm_handler arm_handle_load_store_multiple(u32 instruction) {
  if(bit(instruction, 4)) {
    return &arm_ldm;
  } else {
    return &arm_stm;
  }
}

u32 arm_sdt_shift(u32 instr, registers_t* regs) {
  u8 type = bits(instr, 5, 6);
  u8 amount = bits(instr, 7, 11);
  u32 input = regs->gpr[instr & 0xf];
  bool carry_out = regs->cpsr.carry;
  u32 result = shift_imm(type, input, amount, &carry_out, regs);
  return result;
}

ARM_INSTRUCTION(stm) {
  u8 rn = bits(instr, 16, 19);
  u32 base_address = registers->gpr[rn];

  u16 list_mask = instr & 0xffff;
  
  bool increment = bit(instr, 23);
  bool before = bit(instr, 24);

  u8 count_regs = 0;
  for(u8 i = 0; i < 15; i++) {
    if(bit(list_mask, i)) {
      if(before) {
        base_address += increment ? 4 : -4;
      }
      write_32(mem, registers->gpr[PC], base_address, registers->gpr[i]);
      if(!before) {
        base_address += increment ? 4 : -4;
      }

      count_regs++;
    }
  }

  if(W(instr)) {
    registers->gpr[rn] += U(instr) ? (count_regs * 4) : -(count_regs * 4);
  }
}

ARM_INSTRUCTION(ldm) {
  u8 rn = bits(instr, 16, 19);
  u32 base_address = registers->gpr[rn];

  u16 list_mask = instr & 0xffff;
  
  bool increment = bit(instr, 23);
  bool before = bit(instr, 24);

  u8 count_regs = 0;
  for(u8 i = 0; i < 15; i++) {
    if(bit(list_mask, i)) {
      if(before) {
        base_address += increment ? 4 : -4;
      }
      registers->gpr[i] = read_32(mem, registers->gpr[PC], base_address);
      if(!before) {
        base_address += increment ? 4 : -4;
      }
      count_regs++;
    }
  }

  if(W(instr)) {
    registers->gpr[rn] += U(instr) ? (count_regs * 4) : -(count_regs * 4);
  }
}

ARM_INSTRUCTION(strh) {
  bool I = B(instr); // STRH/LDRH's 'I' is STR/LDR's 'B'
  u32 address = registers->gpr[rn(instr)];
  u32 offset = I ? (bits(instr, 8, 11) << 4) | (instr & 0xf) : registers->gpr[rm(instr)];

  if(P(instr)) {
    address = U(instr) ? address + offset : address - offset;
  }

  address &= ~1;

  write_16(mem, registers->gpr[PC], address, registers->gpr[rd(instr)]);

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
  bool I = B(instr); // STRH/LDRH's 'I' is STR/LDR's 'B'
  u32 address = registers->gpr[rn(instr)];
  u32 offset = I ? (bits(instr, 8, 11) << 4) | (instr & 0xf) : registers->gpr[rm(instr)];

  address &= ~1;

  

  if(P(instr)) {
    address = U(instr) ? address + offset : address - offset;
  }

  if(rd(instr) != 15) {
    registers->gpr[rd(instr)] = read_16(mem, registers->gpr[PC], address);
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
  u32 address = registers->gpr[rn(instr)];
  u32 offset = I(instr) ? arm_sdt_shift(instr, registers) : instr & 0xfff;

  if(P(instr)) {
    address = U(instr) ? address + offset : address - offset;
  }

  address &= ~3;

  

  if(B(instr)) {
    logfatal("strb!\n");
  } else {
    write_32(mem, registers->gpr[PC], address, registers->gpr[rd(instr)]);
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
  u32 address = registers->gpr[rn(instr)];
  u32 offset = I(instr) ? arm_sdt_shift(instr, registers) : instr & 0xfff;

  if(P(instr)) {
    address = U(instr) ? address + offset : address - offset;
  }

  if(B(instr)) {
    
    if(rd(instr) != 15) {
      registers->gpr[rd(instr)] = read_8(mem, registers->gpr[PC], address);
    } else {
      flush_pipe_32(registers, mem);
    }
  } else {
    address &= ~3;
    
    if(rd(instr) != 15) {
      registers->gpr[rd(instr)] = read_32(mem, registers->gpr[PC], address);
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
  logfatal("Undefined single data transfer instruction: (%08X) (%s)\n", instr, binary_str(instr, 32));
}

ARM_INSTRUCTION(unimplemented_single_data_transfer) {
  logfatal("Unimplemented single data transfer instruction: (%08X) (%s)\n", instr, binary_str(instr, 32));
}
