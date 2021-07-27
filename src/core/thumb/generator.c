#include "thumb/generator.h"

void generate_thumb_lut(thumb_handler thumb_lut[1024]) {
  for(int i = 0; i < 1024; i++) {
    thumb_lut[i] = &thumb_undefined;
  }
}