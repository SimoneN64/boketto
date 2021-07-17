#include "helpers.h"

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
    case 0b1111: return false;
  }
}