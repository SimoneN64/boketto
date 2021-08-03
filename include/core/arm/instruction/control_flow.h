#pragma once
#include "register.h"

ARM_INSTRUCTION(b);
ARM_INSTRUCTION(bx);

void set_pc(bool link, mem_t* mem, registers_t* registers, u32 value);
