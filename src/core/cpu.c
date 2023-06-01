#include "cpu.h"
#include "log.h"

void init_cpu(cpu_t* cpu) {
  init_registers(&cpu->regs);
  generate_arm_lut(cpu->arm_lut);
  generate_thumb_lut(cpu->thumb_lut);
}

void step_cpu(cpu_t* cpu, mem_t* mem) {
  if(cpu->regs.cpsr.thumb) {
    cpu->regs.instruction = fetch_16(&cpu->regs, mem);
    size_t index = cpu->regs.instruction >> 6;
    (cpu->thumb_lut[index])(&cpu->regs, mem);
  } else {
    cpu->regs.instruction = fetch_32(&cpu->regs, mem);
    if(get_condition(cpu->regs.cpsr, cpu->regs.instruction >> 28)) {
      size_t index = ((cpu->regs.instruction >> 16) & 0xFF0) 
                    | ((cpu->regs.instruction >> 4) & 0xF);
      (cpu->arm_lut[index])(&cpu->regs, mem);
    }
  }
}