#include <generator.h>

void generate_arm_lut(arm_handler arm_lut[4096]) {
  for(u32 i = 0; i < 4096; i++) {
    if ((i & 0xE00) == 0xA00) {
      arm_lut[i] = &arm_b;
    } else if ((i & 0xFFD) == 0x121) {
      arm_lut[i] = &arm_bx;
    } else if ((i & 0xC01) == 0) {
      if ((bits(i, 4, 8) & 0b11001) == 0b10000) {
        arm_lut[i] = bit(i, 5) ? &arm_msr : &arm_mrs;
      } else {
        arm_lut[i] = arm_handle_data_processing(bits(i, 5, 8));
      }
    } else if ((i & 0xE00) == 0x200) {
      arm_lut[i] = arm_handle_data_processing(bits(i, 5, 8));
    } else if ((i & 0xC00) == 0x400 || (i & 0xC00) == 0) { // single data transfer
      arm_lut[i] = arm_handle_single_data_transfer(i);
    } else if ((i & 0xE00) == 0x800) {
      arm_lut[i] = arm_handle_load_store_multiple(i);
    } else {
      arm_lut[i] = &arm_unimplemented;
    }
  }
}