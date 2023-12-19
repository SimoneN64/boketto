#include <cpu.h>
#include <log.h>

void init_cpu(cpu_t* cpu) {
  init_registers(&cpu->regs);

  generate_arm_lut(cpu->arm_lut);
  generate_thumb_lut(cpu->thumb_lut);
}

void step_cpu(cpu_t* cpu, mem_t* mem) {
  if(cpu->regs.cpsr.thumb) {
    u16 instruction = fetch_16(&cpu->regs, mem);
    u8 index = instruction >> 8;
    (cpu->thumb_lut[index])(instruction, &cpu->regs, mem);
  } else {
    u32 instruction = fetch_32(&cpu->regs, mem);
    if(get_condition(cpu->regs.cpsr, instruction >> 28)) {
      size_t index = ((instruction >> 16) & 0xFF0) 
                    | ((instruction >> 4) & 0xF);
      (cpu->arm_lut[index])(instruction, &cpu->regs, mem);
    }
  }
}