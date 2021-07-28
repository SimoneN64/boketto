#include "arm/instructions.h"
#include "log.h"

ARM_INSTRUCTION(undefined) {
  logfatal("Undefined arm instruction: (%08X) (%s)\n", registers->instruction, binary_str(registers->instruction));
}

ARM_INSTRUCTION(unimplemented) {
  logfatal("Unimplemented arm instruction: (%08X) (%s)\n", registers->instruction, binary_str(registers->instruction));
}