#pragma once
#include "register.h"
#include <log.h>

#define SHIFT(name) static inline u32 name(u32 input, u8 amount, bool* carry_out, registers_t* registers)

enum {
  lsl = 0b00,
  lsr = 0b01,
  asr = 0b10,
  ror = 0b11,
};

bool get_condition(psr_t psr, u8 cond);
void set_pc(bool link, mem_t* mem, registers_t* registers, u32 value, bool cond);
u32 shift_imm(u8 type, u32 input, u8 amount, bool* carry_out, registers_t* registers);
u32 shift_reg(u8 type, u32 input, u8 amount, bool* carry_out, registers_t* registers);