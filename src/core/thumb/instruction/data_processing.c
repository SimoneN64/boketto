#include "thumb/instruction/data_processing.h"
#include "log.h"

thumb_handler thumb_handle_data_processing(u16 instruction) {
  u8 opcode = bits(instruction, 6, 9);
  switch(opcode) {
  case 0x1: return &thumb_ldr;
  default: return &thumb_unimplemented_data_processing;
  }
}

THUMB_INSTRUCTION(unimplemented_data_processing) {
  logfatal("Unimplemented thumb data processing instruction %02X\n", registers->instruction);
}

THUMB_INSTRUCTION(ldr) {
  logfatal("Wait a sec\n");
}