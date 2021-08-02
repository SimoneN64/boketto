#pragma once
#include "register.h"

u32 shift_single_data_transfer(registers_t* regs);
arm_handler arm_handle_single_data_transfer(u32 instruction);
ARM_INSTRUCTION(str);
ARM_INSTRUCTION(ldr);
ARM_INSTRUCTION(undefined_single_data_transfer);
ARM_INSTRUCTION(unimplemented_single_data_transfer);