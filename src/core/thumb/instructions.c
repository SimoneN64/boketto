#include "thumb/instructions.h"
#include "log.h"

THUMB_INSTRUCTION(undefined) {
  logfatal("Undefined thumb instruction: (%08X) (%s)\n", registers->instruction, binary_str(registers->instruction));
}

THUMB_INSTRUCTION(unimplemented) {
  logfatal("Unimplemented thumb instruction: (%08X) (%s)\n", registers->instruction, binary_str(registers->instruction));
}