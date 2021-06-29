#include "cpu.h"

namespace boketto::core
{
Cpu::ARMHandler Cpu::Misc(u32 instruction) {
  printf("Unimplemented misc %08X\n", instruction);
  exit(1);
}

}