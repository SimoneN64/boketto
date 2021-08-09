#pragma once
#define PC 15
#define LR 14

#include "mem.h"
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
  u32 gpr[16], gpr_t[11], pipe[2], instruction;
  psr_t cpsr, spsr;
} registers_t;

void init_registers(registers_t* registers);
void flush_pipe_32(registers_t* registers, mem_t* mem);
void flush_pipe_16(registers_t* registers, mem_t* mem);
u32 fetch_32(registers_t* regs, mem_t* mem);
u16 fetch_16(registers_t* regs, mem_t* mem);

typedef void (*arm_handler)(registers_t*, mem_t*);
typedef void (*thumb_handler)(registers_t*, mem_t*);

#define ARM_INSTRUCTION(name) void arm_##name(registers_t* registers, mem_t* mem)
#define THUMB_INSTRUCTION(name) void thumb_##name(registers_t* registers, mem_t* mem)