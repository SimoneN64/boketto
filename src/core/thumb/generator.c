#include "thumb/generator.h"

void generate_thumb_lut(thumb_handler thumb_lut[1024]) {
  for(u16 i = 0; i < 1024; i++) {
    u16 mask = i << 6;
    if((mask & 0xFC00) == 0x4000) {
      thumb_lut[i] = thumb_handle_data_processing(mask);
    } else {
      thumb_lut[i] = &thumb_undefined;
    }
  }
}