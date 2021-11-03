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
    case 0b1111:
      logfatal("NV condition should never happen!\n");
    default:
      logfatal("Huh?!\n");
  }
}

void set_pc(bool link, mem_t* mem, registers_t* registers, u32 value, bool cond) {
  if(link) {
    registers->gpr[LR] = registers->gpr[PC] - (registers->cpsr.thumb ? 1 : 4);
  }

  registers->gpr[PC] = value;

  if(cond) {
    registers->gpr[PC] &= ~1;
    flush_pipe_16(registers, mem);
  } else {
    registers->gpr[PC] &= ~3;
    flush_pipe_32(registers, mem);
  }
}

SHIFT(LSL) {
  u32 output = 0;
  if (amount == 0) {
    output = input;
  } else if (amount < 32) {
    output = input << amount;
    *carry_out = input & (1 << (32 - amount));
  } else if (amount == 32) {
    output = 0;
    *carry_out = input & 1;
  } else {
    output = 0;
    *carry_out = false;
  }

  return output;
}

SHIFT(LSR_IMM) {
  u32 output;
  if (amount == 0) {
    output = 0;
    *carry_out = input >> 31;
  } else if (amount < 32) {
    output = input >> amount;
    *carry_out = input & (1 << (amount - 1));
  } else if (amount == 32) {
    logfatal("How did you even get an amount of 32 in LSR_IMM?!\n");
  } else {
    output = 0;
    *carry_out = false;
  }

  return output;
}

SHIFT(LSR_REG) {
  u32 output;
  if (amount == 0) {
    output = input;
  } else if (amount < 32) {
    output = input >> amount;
    *carry_out = input & (1 << (amount - 1));
  } else if (amount == 32) {
    output = 0;
    *carry_out = input & (1 << 31);
  } else {
    output = 0;
    *carry_out = false;
  }

  return output;
}

SHIFT(ASR_IMM) {
  u32 output = 0;
  if (amount < 32) {
    output = (u32)((s32)input >> amount);
    *carry_out = input >> (amount - 1);
  } else {
    output = (u32)((s32)input >> 31);
    *carry_out = input >> 31;
  }

  return output;
}

SHIFT(ASR_REG) {
  u32 output = 0;
  if (amount == 0) {
    output = input;
  } else if (amount < 32) {
    output = (u32)((s32)input >> amount);
    *carry_out = input >> (amount - 1);
  } else {
    output = (u32)((s32)input >> 31);
    *carry_out = input >> 31;
  }

  return output;
}

SHIFT(ROR_IMM) {
  u32 output = 0;
  if (amount == 0) {
    output = (input >> 1) | ((u32)registers->cpsr.carry << 31);
    *carry_out = input & 1;
  } else {
    if ((amount & 31) != 0) {
      output = ror32(input, amount & 31);
    } else {
      output = input;
    }
    *carry_out = input & (1 << ((amount - 1) & 31));
  }

  return output;
}

SHIFT(ROR_REG) {
  u32 output = 0;
  if (amount == 0) {
    output = input;
  } else {
    if ((amount & 31) != 0) {
      output = ror32(input, amount & 31);
    } else {
      output = input;
    }
    *carry_out = input & (1 << ((amount - 1) & 31));
  }

  return output;
}

u32 shift_imm(u8 type, u32 input, u8 amount, bool* carry_out, registers_t* registers) {
  switch(type) {
    case lsl: return LSL(input, amount, carry_out, registers);
    case lsr: return LSR_IMM(input, amount, carry_out, registers);
    case asr: return ASR_IMM(input, amount, carry_out, registers);
    case ror: return ROR_IMM(input, amount, carry_out, registers);
    default: logfatal("The fuck?!\n");
  }
}

u32 shift_reg(u8 type, u32 input, u8 amount, bool* carry_out, registers_t* registers) {
  switch(type) {
    case lsl: return LSL(input, amount, carry_out, registers);
    case lsr: return LSR_REG(input, amount, carry_out, registers);
    case asr: return ASR_REG(input, amount, carry_out, registers);
    case ror: return ROR_REG(input, amount, carry_out, registers);
    default: logfatal("The fuck?!\n");
  }
}