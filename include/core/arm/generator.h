#pragma once
#include "instructions.h"

typedef void (*arm_handler)(u32);
typedef void (*thumb_handler)(u16);

void generate_arm_lut(arm_handler arm_lut[4096]);