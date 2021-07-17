#pragma once
#include "arm/generator.h"
#include "helpers.h"

typedef struct {
  arm_handler arm_lut[4096];
  thumb_handler thumb_lut[1024];
  registers_t regs;
} cpu_t;

void init_cpu(cpu_t* cpu);
void step(cpu_t* cpu);