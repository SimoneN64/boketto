#include "register.h"

void init_registers(registers_t* registers) {
  memset(registers->gpr, 0, sizeof(registers->gpr));
  registers->gpr[PC] = 0x08000000;
  registers->cpsr.raw = 0;
  registers->spsr.raw = 0;
}