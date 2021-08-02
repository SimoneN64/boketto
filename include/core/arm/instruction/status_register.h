#pragma once
#include "register.h"

arm_handler arm_handle_status_register(u32 instruction);

ARM_INSTRUCTION(msr);
ARM_INSTRUCTION(mrs);