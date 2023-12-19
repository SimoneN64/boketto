#include <arm/instructions.h>
#include <log.h>

ARM_INSTRUCTION(undefined) {
  logfatal("Undefined arm instruction: (%08X) (%s)\n", instr, binary_str(instr, 32));
}

ARM_INSTRUCTION(unimplemented) {
  logfatal("Unimplemented arm instruction: (%08X) (%s)\n", instr, binary_str(instr, 32));
}