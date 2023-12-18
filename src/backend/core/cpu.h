#pragma once
#include <arm/generator.h>
#include <thumb/generator.h>
#include <helpers.h>
#include <mem.h>

typedef struct {
  arm_handler arm_lut[4096];
  thumb_handler thumb_lut[256];
  registers_t regs;
} cpu_t;

void init_cpu(cpu_t* cpu);
void step_cpu(cpu_t* cpu, mem_t* mem);