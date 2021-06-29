#pragma once
#include "mem.h"
#define LR 14
#define PC 15

namespace boketto::core
{
class Cpu {
public:
  Cpu(std::string bios);
  void step();

  Mem mem;
private:
  union {
    struct {
      unsigned mode:5;
      unsigned thumb:1;
      unsigned fiq:1;
      unsigned irq:1;
      unsigned:20;
      unsigned overflow:1;
      unsigned carry:1;
      unsigned zero:1;
      unsigned negative:1;
    };

    u32 raw;
  } cpsr, spsr;

  u32 gpr[31];
  u32 next_pc;

  enum Flags {
    Overflow,
    Carry,
    Zero,
    Negative
  };

  template <Flags flag>
  bool GetFlag() {
    switch(flag) {
      case Overflow: return cpsr.overflow;
      case Carry: return cpsr.carry;
      case Zero: return cpsr.zero;
      case Negative: return cpsr.negative;
    }
  }
  
  bool GetCondition(u8 cond) {
    switch(cond & 0xf) {
      case 0b0000: return GetFlag<Zero>();
      case 0b0001: return !GetFlag<Zero>();
      case 0b0010: return GetFlag<Carry>();
      case 0b0011: return !GetFlag<Carry>();
      case 0b0100: return GetFlag<Negative>();
      case 0b0101: return !GetFlag<Negative>();
      case 0b0110: return GetFlag<Overflow>();
      case 0b0111: return !GetFlag<Overflow>();
      case 0b1000: return GetFlag<Carry>() && !GetFlag<Zero>();
      case 0b1001: return !GetFlag<Carry>() && GetFlag<Zero>();
      case 0b1010: return GetFlag<Negative>() == GetFlag<Overflow>();
      case 0b1011: return GetFlag<Negative>() != GetFlag<Overflow>();
      case 0b1100: return !GetFlag<Zero>() && (GetFlag<Negative>() == GetFlag<Overflow>());
      case 0b1101: return GetFlag<Zero>() || (GetFlag<Negative>() != GetFlag<Overflow>());
      case 0b1110: return true;
      case 0b1111: return false;
    }
  }

  void GenerateARMLut();
  void GenerateThumbLut();

  using ARMHandler = void (Cpu::*) (u32);
  using ThumbHandler = void (Cpu::*) (u16);

  ARMHandler Misc(u32 instruction);
  ARMHandler DataProcessing(u32 instruction);
  ARMHandler Branch(u32 instruction);

  void b(u32 instruction);

  void ARMUndefined(u32 instruction) {
    printf("Undefined instruction: %08X\n", instruction);
    exit(1);
  }

  void ThumbUndefined(u16 instruction) {
    printf("Undefined instruction: %04X\n", instruction);
    exit(1);
  }

  std::array<ARMHandler, 4096> arm_lut;
  std::array<ThumbHandler, 1024> thumb_lut;
};
}