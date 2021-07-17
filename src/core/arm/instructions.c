#include "arm/instructions.h"

void arm_undefined(u32 instruction) {
  printf("Undefined instruction: %08X\n", instruction);
  exit(1);
}