#include <register.h>
#include <log.h>

void init_registers(registers_t* registers) {
  memset(registers, 0, sizeof(registers_t));
  registers->cpsr.raw = 0x6000001F;
  registers->gpr[0] = 0x08000000;
  registers->gpr[1] = 0x000000EA;
  registers->gpr[13] = 0x03007F00;

  registers->gpr[PC] = 0x08000000;
}

void flush_pipe_32(registers_t* regs, mem_t* mem) {
  
  regs->pipe[0] = read_32(mem, regs->gpr[PC], regs->gpr[PC]);
  regs->pipe[1] = read_32(mem, regs->gpr[PC], regs->gpr[PC] + 4);
  regs->gpr[PC] += 4;
}

void flush_pipe_16(registers_t* regs, mem_t* mem) {
  
  regs->pipe[0] = read_16(mem, regs->gpr[PC], regs->gpr[PC]);
  regs->pipe[1] = read_16(mem, regs->gpr[PC], regs->gpr[PC] + 2);
  regs->gpr[PC] += 2;
}

u32 fetch_32(registers_t* regs, mem_t* mem) {
  regs->gpr[PC] += 4;
  u32 instruction = regs->pipe[0];
  regs->pipe[0] = regs->pipe[1];
  regs->pipe[1] = read_32(mem, regs->gpr[PC], regs->gpr[PC]);
  return instruction;
}

u16 fetch_16(registers_t* regs, mem_t* mem) {
  regs->gpr[PC] += 2;
  u16 instruction = regs->pipe[0];
  regs->pipe[0] = regs->pipe[1];
  regs->pipe[1] = read_16(mem, regs->gpr[PC], regs->gpr[PC]);
  return instruction;
}

u8 mode_index(u8 mode) {
  switch(mode) {
  case 0x10: case 0x1F: return 0;
  case 0x11 ... 0x13: return mode & 0xF;
  case 0x17: return 4;
  case 0x1B: return 5;
  default: logfatal("Invalid CPU mode %02X!\n", mode);
  }
}

void change_mode(registers_t* regs, u8 new_mode) {
  u8 curr_mode = regs->cpsr.mode;
  if(curr_mode == new_mode) {
    return;
  }

  regs->cpsr.mode = new_mode;

  switch(curr_mode) {
  case 0x10: case 0x1F:
    regs->r13_banks[0] = regs->gpr[13];
    regs->r14_banks[0] = regs->gpr[14];
    break;
  case 0x11:
    regs->r8_banks[1] = regs->gpr[8];
    regs->r9_banks[1] = regs->gpr[9];
    regs->r10_banks[1] = regs->gpr[10];
    regs->r11_banks[1] = regs->gpr[11];
    regs->r12_banks[1] = regs->gpr[12];
    break;
  default:
    regs->r8_banks[0] = regs->gpr[8];
    regs->r9_banks[0] = regs->gpr[9];
    regs->r10_banks[0] = regs->gpr[10];
    regs->r11_banks[0] = regs->gpr[11];
    regs->r12_banks[0] = regs->gpr[12];

    u8 curr_mode_index = mode_index(curr_mode);
    regs->r13_banks[curr_mode_index] = regs->gpr[13];
    regs->r14_banks[curr_mode_index] = regs->gpr[14];
    regs->spsr_banks[curr_mode_index - 1].raw = regs->spsr.raw;
    break;
  }

  switch(new_mode) {
  case 0x10: case 0x1F:
    regs->gpr[13] = regs->r13_banks[0];
    regs->gpr[14] = regs->r14_banks[0];
    break;
  case 0x11:
    regs->gpr[8] = regs->r8_banks[1];
    regs->gpr[9] = regs->r9_banks[1];
    regs->gpr[10] = regs->r10_banks[1];
    regs->gpr[11] = regs->r11_banks[1];
    regs->gpr[12] = regs->r12_banks[1];
    break;
  default:
    regs->gpr[8] = regs->r8_banks[0];
    regs->gpr[9] = regs->r9_banks[0];
    regs->gpr[10] = regs->r10_banks[0];
    regs->gpr[11] = regs->r11_banks[0];
    regs->gpr[12] = regs->r12_banks[0];

    u8 new_mode_index = mode_index(new_mode);
    regs->gpr[13] = regs->r13_banks[new_mode_index];
    regs->gpr[14] = regs->r14_banks[new_mode_index];
    regs->spsr.raw = regs->spsr_banks[new_mode_index - 1].raw;
    break;
  }
}