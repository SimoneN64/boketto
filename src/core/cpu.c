#include "cpu.h"

void init_cpu(cpu_t* cpu) {
  init_registers(&cpu->regs);
  generate_arm_lut(cpu->arm_lut);
}

void step_cpu(cpu_t* cpu, mem_t* mem) {
  if(cpu->regs.cpsr.thumb) {
    cpu->regs.gpr[PC] += 2;
    u16 instruction = read_16(mem, cpu->regs.gpr[PC] - 4);
    cpu->regs.instruction = instruction;
    (cpu->thumb_lut[instruction >> 6])(&cpu->regs);
  } else {
    cpu->regs.gpr[PC] += 4;
    u32 instruction = read_32(mem, cpu->regs.gpr[PC] - 8);
    cpu->regs.instruction = instruction;
    if(get_condition(cpu->regs.cpsr, instruction >> 28)) {
      (cpu->arm_lut[((instruction >> 16) & 0xFF0) | ((instruction >> 4) & 0xF)])(&cpu->regs);
    }
  }
}