#pragma once
#include "register.h"

bool get_condition(psr_t psr, u8 cond);
void set_pc(bool link, mem_t* mem, registers_t* registers, u32 value);