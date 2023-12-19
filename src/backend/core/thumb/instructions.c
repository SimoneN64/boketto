#include <thumb/instructions.h>
#include <log.h>

THUMB_INSTRUCTION(undefined) {
  logfatal("Undefined thumb instruction: (%04X) (%s)\n", instr, binary_str(instr, 16));
}

THUMB_INSTRUCTION(unimplemented) {
  logfatal("Unimplemented thumb instruction: (%04X) (%s)\n", instr, binary_str(instr, 16));
}