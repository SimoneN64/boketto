#pragma once
#include "instructions.h"

void generate_arm_lut(arm_handler arm_lut[4096]);
void generate_thumb_lut(thumb_handler thumb_lut[4096]);