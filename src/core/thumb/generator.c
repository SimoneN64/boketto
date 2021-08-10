#include "thumb/generator.h"

void generate_thumb_lut(thumb_handler thumb_lut[1024]) {
  for(u16 i = 0; i < 1024; i++) {
    u16 mask = i << 6;
    if((mask & 0xF800) == 0) {
      thumb_lut[i] = &thumb_lsl;
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
    } else if((mask & 0xF800) == 0x4800) {
      thumb_lut[i] = &thumb_ldr;
    } else {
      thumb_lut[i] = &thumb_undefined;
    }
  }
}