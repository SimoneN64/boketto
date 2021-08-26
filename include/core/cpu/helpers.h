#pragma once
#include "register.h"
#define SHIFT(name) static inline u32 name(u32 input, u8 amount, bool* carry_out, registers_t* registers)

enum {
  lsl = 0b00,
  lsr = 0b01,
  asr = 0b10,
  ror = 0b11,
};

bool get_condition(psr_t psr, u8 cond);
void set_pc(bool link, mem_t* mem, registers_t* registers, u32 value);
u32 shift_imm(u8 type, u32 input, u8 amount, bool* carry_out, registers_t* registers);
u32 shift_reg(u8 type, u32 input, u8 amount, bool* carry_out, registers_t* registers);

static inline void print_list(u32 instruction) {
#ifdef DEBUG
  u8 rn = (instruction >> 8) & 7;
  u8 list_mask = instruction & 0xff;
  if(bit(instruction, 27)) {
    logdebug("ld");
  } else {
    logdebug("st");
  }

  logdebug("mia r%d!, { ", rn);
  static const char* reg_str[8] = {
      "r0", "r1", "r2", "r3",
      "r4", "r5", "r6", "r7"
  };

  for(int i = 0; i < 8; i++) {
    bool is_end = false;
    if(((list_mask >> (i + 1)) & (0xff >> (i + 1))) == 0) {
      is_end = true;
    }

    if(bit(list_mask, i)) {
      logdebug(!is_end ? "%s, " : "%s", reg_str[i]);
    }
  }

  logdebug(" }\n");
#endif
}