#include "arm/generator.h"

void generate_arm_lut(arm_handler arm_lut[4096]) {
  for(u32 i = 0; i < 4096; i++) {
    u32 mask = ((i & 0xFF0) << 16) | ((i & 0xF) << 4);
    u32 opcode = mask & 0x0FFFFFFF;
    if ((opcode & 0x0E000000) == 0x0A000000) {
      arm_lut[i] = &arm_b;
    } else if ((opcode & 0x0FF000D0) == 0x01200010) {
      arm_lut[i] = &arm_bx;
    } else if ((opcode & 0x0C000010) == 0) {
      if ((bits(opcode, 20, 24) & 0b11001) == 0b10000) {
        arm_lut[i] = arm_handle_status_register(opcode);
      } else {
        arm_lut[i] = arm_handle_data_processing(opcode);
      }
    } else if ((opcode & 0x0E000000) == 0x02000000) {
      arm_lut[i] = arm_handle_data_processing(opcode);
    } else if ((opcode & 0x0C000000) == 0x04000000 || (opcode & 0x0C000000) == 0) { // single data transfer
      arm_lut[i] = arm_handle_single_data_transfer(opcode);
    } else if ((opcode & 0x0E000000) == 0x08000000) {
      arm_lut[i] = arm_handle_load_store_multiple(opcode);
    } else {
      arm_lut[i] = &arm_undefined;
    }
  }
}