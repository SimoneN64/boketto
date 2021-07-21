#include "arm/instructions.h"

ARM_INSTRUCTION(undefined) {
  logfatal("Undefined instruction: (%08X) (%s)\n", registers->instruction, binary_str(registers->instruction));
}

ARM_INSTRUCTION(unimplemented) {
  logfatal("Unimplemented instruction: (%08X) (%s)\n", registers->instruction, binary_str(registers->instruction));
}