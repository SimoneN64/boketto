#pragma once
#include "data_processing.h"

arm_handler arm_handle_single_data_transfer(u32 instruction);
ARM_INSTRUCTION(str);
ARM_INSTRUCTION(ldr);
ARM_INSTRUCTION(undefined_single_data_transfer);
ARM_INSTRUCTION(unimplemented_single_data_transfer);