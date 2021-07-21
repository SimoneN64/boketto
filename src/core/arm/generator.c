#include "arm/generator.h"

void generate_arm_lut(arm_handler arm_lut[4096]) {
  for(u32 i = 0; i < 4096; i++) {
    u32 mask = ((i & 0xFF0) << 16) | ((i & 0xF) << 4);
    u32 opcode = mask & 0x0FFFFFFF;
    if ((opcode & 0x0E000000) == 0x0A000000) {
      arm_lut[i] = &arm_branch;
    } else if ((opcode & 0x0c000010) == 0) {
      if((bits(opcode, 20, 24) & 0b11001) == 0b10000) {
        arm_lut[i] = &arm_unimplemented;
      } else {
        arm_lut[i] = &arm_unimplemented;
      }
    } else {
      arm_lut[i] = &arm_undefined;
    }
  }
}