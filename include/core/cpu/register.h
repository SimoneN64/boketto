#pragma once
#define PC 15
#define LR 14

#include "bit.h"
#include <memory.h>

typedef union {
  struct {
    unsigned mode:5;
    unsigned thumb:1;
    unsigned fiq:1;
    unsigned irq:1;
    unsigned:20;
    unsigned overflow:1;
    unsigned carry:1;
    unsigned zero:1;
    unsigned negative:1;
  };

  u32 raw;
} psr_t;

typedef struct {
  u32 gpr[31];
  psr_t cpsr, spsr;
} registers_t;

void init_registers(registers_t* registers);