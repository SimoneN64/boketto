#pragma once
#include <register.h>
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

static inline void print_list(bool thumb, u32 instruction) {
#ifdef DEBUG
  const char *addr_modes[4] = {
      "da", "ia", "db", "ib"
  };

  u8 rn = thumb ? (instruction >> 8) & 7 : (instruction >> 16) & 0xf;
  u16 list_mask = instruction & (thumb ? 0xff : 0xffff);
  if (thumb) {
    if (bit(instruction, 11)) {
      logdebug("ldm");
    } else {
      logdebug("stm");
    }
  } else {
    if (bit(instruction, 20)) {
      logdebug("ldm");
    } else {
      logdebug("stm");
    }
  }

  static const char *reg_str[16] = {
      "r0", "r1", "r2", "r3",
      "r4", "r5", "r6", "r7",
      "r8", "r9", "r10", "r11",
      "r12", "sp", "lr", "pc",
  };

  logdebug("%s %s!, { ", thumb ? "ia" : addr_modes[bits(instruction, 23, 24)], reg_str[rn]);

  for(int i = 0; i < (thumb ? 8 : 15); i++) {
    bool is_end = false;
    if(((list_mask >> (i + 1)) & ((thumb ? 0xff : 0xffff) >> (i + 1))) == 0) {
      is_end = true;
    }

    if(bit(list_mask, i)) {
      logdebug(!is_end ? "%s, " : "%s", reg_str[i]);
    }
  }

  logdebug(" }\n");
#endif
}