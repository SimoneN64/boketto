#pragma once
#include "common.h"
#include <string.h>
#define mask(len) ((1 << ((len) + 1)) - 1)
#define bits(n, lo, hi) (((n) >> (lo)) & mask((hi) - (lo)))
#define bit(n, pos) (((n) >> (pos)) & 1)

s32 sex(s32 number, u8 amount);
size_t next_power_of_two(size_t number);

char* binary_str(u32 number);
