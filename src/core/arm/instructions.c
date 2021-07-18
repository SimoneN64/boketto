#include "arm/instructions.h"

void arm_undefined(registers_t* registers) {
  printf("Undefined instruction: %08X\n", registers->instruction);
  exit(1);
}