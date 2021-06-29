#include "cpu.h"

namespace boketto::core
{
Cpu::ARMHandler Cpu::DataProcessing(u32 instruction) {
  u8 opcode = (instruction >> 21) & 0xF;
  switch(opcode) {
    default:
      printf("Unimplemented data processing %2X\n", opcode);
      exit(1);
  }
}
}