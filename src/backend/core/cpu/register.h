#pragma once
#define PC 15
#define LR 14

#include <mem.h>

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
  u32 pipe[2];
  u32 gpr[16], r8_banks[2], r9_banks[2], r10_banks[2], r11_banks[2], r12_banks[2];
  u32 r13_banks[6], r14_banks[6];
  psr_t cpsr, spsr, spsr_banks[5];
} registers_t;

void init_registers(registers_t* registers);
void flush_pipe_32(registers_t* registers, mem_t* mem);
void flush_pipe_16(registers_t* registers, mem_t* mem);
u32 fetch_32(registers_t* regs, mem_t* mem);
u16 fetch_16(registers_t* regs, mem_t* mem);
u8 mode_index(u8 mode);
void change_mode(registers_t* regs, u8 new_mode);

typedef void (*arm_handler)(u32, registers_t*, mem_t*);
typedef void (*thumb_handler)(u16, registers_t*, mem_t*);

#define ARM_INSTRUCTION(name) void arm_##name(u32 instr, registers_t* registers, mem_t* mem)
#define THUMB_INSTRUCTION(name) void thumb_##name(u16 instr, registers_t* registers, mem_t* mem)