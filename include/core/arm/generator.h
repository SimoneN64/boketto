#pragma once
#include "instructions.h"

typedef arm_handler void (*)(u32);
typedef thumb_handler void (*)(u16);

void generate_arm_lut(arm_handler arm_lut[4096]);