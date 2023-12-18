#pragma once
#include <register.h>
// Single data transfer bits that determine different behaviour, macro'd because "a little hardcoded values and random constants never hurt anyone" amirite
#define I(x) (bit(x, 25))
#define P(x) (bit(x, 24))
#define U(x) (bit(x, 23))
#define B(x) (bit(x, 22))
#define W(x) (bit(x, 21))
#define L(x) (bit(x, 20))
#define rd(x) (bits(x, 12, 15))
#define rn(x) (bits(x, 16, 19))
#define rm(x) ((x) & 0xf)

arm_handler arm_handle_single_data_transfer(u32 instruction);
arm_handler arm_handle_load_store_multiple(u32 instruction);
u32 arm_sdt_shift(registers_t* regs);

ARM_INSTRUCTION(str);
ARM_INSTRUCTION(ldr);
ARM_INSTRUCTION(stm);
ARM_INSTRUCTION(ldm);
ARM_INSTRUCTION(strh);
ARM_INSTRUCTION(ldrh);
ARM_INSTRUCTION(undefined_single_data_transfer);
ARM_INSTRUCTION(unimplemented_single_data_transfer);