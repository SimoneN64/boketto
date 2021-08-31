#include <thumb/generator.h>

void generate_thumb_lut(thumb_handler thumb_lut[1024]) {
  for(u16 i = 0; i < 1024; i++) {
    u16 mask = i << 6;
    if((mask & 0xF800) == 0) {
      thumb_lut[i] = &thumb_lsl_imm;
    } else if((mask & 0xF800) == 0x0800) {
      thumb_lut[i] = &thumb_lsr_imm;
    } else if((mask & 0xF000) == 0xD000) {
      thumb_lut[i] = &thumb_bcond;
    } else if((mask & 0xFF80) == 0x4700) {
      thumb_lut[i] = &thumb_bx;
    } else if((mask & 0xF800) == 0xF000) {
      thumb_lut[i] = &thumb_bl_high;
    } else if((mask & 0xF800) == 0xF800) {
      thumb_lut[i] = &thumb_bl_low;
    } else if((mask & 0xFFC0) == 0x4380) {
      thumb_lut[i] = &thumb_bic;
    } else if((mask & 0xF800) == 0x2000) {
      thumb_lut[i] = &thumb_mov_imm;
    } else if((mask & 0xFE00) == 0x1800) {
      thumb_lut[i] = &thumb_add_reg;
    } else if((mask & 0xF800) == 0x3000) {
      thumb_lut[i] = &thumb_add_imm;
    } else if((mask & 0xFE00) == 0x1A00) {
      thumb_lut[i] = &thumb_sub_reg;
    } else if((mask & 0xF800) == 0x3800) {
      thumb_lut[i] = &thumb_sub_imm;
    } else if((mask & 0xF800) == 0x2800) {
      thumb_lut[i] = &thumb_cmp_imm;
    } else if((mask & 0xF800) == 0x4800) {
      thumb_lut[i] = &thumb_ldr_pc;
    } else if((mask & 0xF800) == 0x6800) {
      thumb_lut[i] = &thumb_ldr_reg;
    } else if((mask & 0xF800) == 0x6000) {
      thumb_lut[i] = &thumb_str_reg;
    } else if((mask & 0xF800) == 0xC800) {
      thumb_lut[i] = &thumb_ldmia;
    } else if((mask & 0xF800) == 0xC000) {
      thumb_lut[i] = &thumb_stmia;
    } else if((mask & 0xF800) == 0x1000) {
      thumb_lut[i] = &thumb_asr_imm;
    } else if((mask & 0xF800) == 0x8000) {
      thumb_lut[i] = &thumb_strh_imm;
    } else if((mask & 0xFE00) == 0xB400) {
      thumb_lut[i] = &thumb_push;
    } else if((mask & 0xFE00) == 0xBC00) {
      thumb_lut[i] = &thumb_pop;
    } else if((mask & 0xFF00) == 0x4600) {
      thumb_lut[i] = &thumb_mov_high;
    } else if((mask & 0xFFC0) == 0x4180) {
      thumb_lut[i] = &thumb_sbc;
    } else {
      thumb_lut[i] = &thumb_undefined;
    }
  }
}