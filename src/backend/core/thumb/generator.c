#include <thumb/generator.h>

void generate_thumb_lut(thumb_handler thumb_lut[256]) {
  for(u16 i = 0; i < 256; i++) {
    if((i >> 3) >= 0x0 && (i >> 3) <= 0x2) {
      thumb_lut[i] = &thumb_unimplemented; // msr
    } else if((i >> 1) == 0xC) {
      thumb_lut[i] = &thumb_add_reg;
    } else if((i >> 1) == 0xE) {
      thumb_lut[i] = &thumb_add_imm;
    } else if((i >> 1) == 0xD) {
      thumb_lut[i] = &thumb_sub_reg;
    } else if((i >> 1) == 0xF) {
      thumb_lut[i] = &thumb_sub_imm;
    } else if((i >> 5) == 0x1) {
      switch((i >> 3) & 3) {
        case 0: thumb_lut[i] = &thumb_mov_imm; break;
        case 1: thumb_lut[i] = &thumb_cmp_imm; break;
        case 2: thumb_lut[i] = &thumb_add_imm; break;
        case 3: thumb_lut[i] = &thumb_sub_imm; break;
        default: thumb_lut[i] = &thumb_undefined;
      }
    } else if((i >> 2) == 0x10) {

    } else {
      thumb_lut[i] = &thumb_undefined;
    }
  }
}