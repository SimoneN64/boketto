#include "register.h"

void init_registers(registers_t* registers) {
  memset(registers->gpr, 0, sizeof(registers->gpr));
  memset(registers->pipe, 0, sizeof(registers->pipe));
  registers->cpsr.raw = 0;
  registers->spsr.raw = 0;

  registers->gpr[PC] = 0x08000000;
}

void flush_pipe_32(registers_t* regs, mem_t* mem) {
  regs->pipe[0] = read_32(mem, regs->gpr[PC]);
  regs->pipe[1] = read_32(mem, regs->gpr[PC] + 4);
  regs->gpr[PC] += 4;
}

void flush_pipe_16(registers_t* regs, mem_t* mem) {
  regs->pipe[0] = read_16(mem, regs->gpr[PC]);
  regs->pipe[1] = read_16(mem, regs->gpr[PC] + 2);
  regs->gpr[PC] += 2;
}

u32 fetch_32(registers_t* regs, mem_t* mem) {
  regs->gpr[PC] += 4;
  u32 instruction = regs->pipe[0];
  regs->pipe[0] = regs->pipe[1];
  regs->pipe[1] = read_32(mem, regs->gpr[PC]);
  return instruction;
}

u16 fetch_16(registers_t* regs, mem_t* mem) {
  regs->gpr[PC] += 2;
  u32 instruction = regs->pipe[0];
  regs->pipe[0] = regs->pipe[1];
  regs->pipe[1] = read_16(mem, regs->gpr[PC]);
  return instruction;
}
