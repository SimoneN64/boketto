#pragma once
#include "register.h"

u32 shift_data_processing(registers_t* regs);
arm_handler arm_handle_data_processing_shift(u32 instruction);
arm_handler arm_handle_data_processing(u32 instruction);
ARM_INSTRUCTION(mov);
ARM_INSTRUCTION(undefined_data_processing);
ARM_INSTRUCTION(unimplemented_data_processing);