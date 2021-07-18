#pragma once
#include "instructions.h"

typedef void (*arm_handler)(registers_t*);
typedef void (*thumb_handler)(registers_t*);

void generate_arm_lut(arm_handler arm_lut[4096]);