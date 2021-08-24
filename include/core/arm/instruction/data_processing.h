#pragma once
#include "register.h"

arm_handler arm_handle_data_processing(u32 instruction);
u32 arm_data_processing_shift(registers_t* regs, bool* carry_out);

ARM_INSTRUCTION(mov);
ARM_INSTRUCTION(add);
ARM_INSTRUCTION(cmp);
ARM_INSTRUCTION(undefined_data_processing);
ARM_INSTRUCTION(unimplemented_data_processing);