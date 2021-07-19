#include "arm/instructions.h"

void arm_undefined(registers_t* registers) {
  logfatal("Undefined instruction: (%08X) (%s)\n", registers->instruction, binary_str(registers->instruction));
}