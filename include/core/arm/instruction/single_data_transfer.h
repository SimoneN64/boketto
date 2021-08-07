#pragma once
#include "register.h"
// Single data transfer bits that determine different behaviour, macro'd because "a little hardcoded values and random constants never hurt anyone" amirite
#define P(x) (bit(x, 24))
#define U(x) (bit(x, 23))
#define I(x) (bit(x, 22))
#define W(x) (bit(x, 21))
#define L(x) (bit(x, 20))
#define rd(x) (bits(x, 12, 15))
#define rn(x) (bits(x, 16, 19))

u32 shift_single_data_transfer(registers_t* regs);
arm_handler arm_handle_single_data_transfer(u32 instruction);
ARM_INSTRUCTION(str);
ARM_INSTRUCTION(ldr);
ARM_INSTRUCTION(strh);
ARM_INSTRUCTION(ldrh);
ARM_INSTRUCTION(undefined_single_data_transfer);
ARM_INSTRUCTION(unimplemented_single_data_transfer);