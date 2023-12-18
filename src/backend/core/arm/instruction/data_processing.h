#pragma once
#include <register.h>

arm_handler arm_handle_data_processing(u8);
u32 arm_data_processing_shift(registers_t* regs, bool* carry_out);

ARM_INSTRUCTION(mov);
ARM_INSTRUCTION(add);
ARM_INSTRUCTION(and);
ARM_INSTRUCTION(orr);
ARM_INSTRUCTION(tst);
ARM_INSTRUCTION(cmp);
ARM_INSTRUCTION(undefined_data_processing);
ARM_INSTRUCTION(unimplemented_data_processing);