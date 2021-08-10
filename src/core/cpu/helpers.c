#include "helpers.h"
#include "log.h"

bool get_condition(psr_t psr, u8 cond) {
  switch(cond & 0xf) {
    case 0b0000: return  psr.zero;
    case 0b0001: return !psr.zero;
    case 0b0010: return  psr.carry;
    case 0b0011: return !psr.carry;
    case 0b0100: return  psr.negative;
    case 0b0101: return !psr.negative;
    case 0b0110: return  psr.overflow;
    case 0b0111: return !psr.overflow;
    case 0b1000: return  psr.carry && !psr.zero;
    case 0b1001: return !psr.carry &&  psr.zero;
    case 0b1010: return  psr.negative == psr.overflow;
    case 0b1011: return  psr.negative != psr.overflow;
    case 0b1100: return !psr.zero && (psr.negative == psr.overflow);
    case 0b1101: return  psr.zero || (psr.negative != psr.overflow);
    case 0b1110: return true;
    case 0b1111: logfatal("NV condition should never happen!\n");
  }
}

void set_pc(bool link, mem_t* mem, registers_t* registers, u32 value) {
  if(link) {
    registers->gpr[LR] = registers->gpr[PC] - 4;
  }

  registers->gpr[PC] = value;

  if(registers->cpsr.thumb) {
    registers->gpr[PC] &= ~1;
    flush_pipe_16(registers, mem);
  } else {
    registers->gpr[PC] &= ~3;
    flush_pipe_32(registers, mem);
  }
}