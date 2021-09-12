#pragma once
#include "common.h"
#include <string.h>
#define mask(len) ((1 << ((len) + 1)) - 1)
#define bits(n, lo, hi) (((n) >> (lo)) & mask((hi) - (lo)))
#define bit(n, pos) (((n) >> (pos)) & 1)
#define ror8(n, amt) (((n) >> (amt)) | ((n) << (8 - (amt))))
#define ror16(n, amt) (((n) >> (amt)) | ((n) << (16 - (amt))))
#define ror32(n, amt) (((n) >> (amt)) | ((n) << (32 - (amt))))
#define color5_to_8(x) ((((x) & 0x1f) << 3) | (((x) & 0x1f) >> 2))

s32 sign_extend32(s32 number, u8 amount);
size_t next_power_of_two(size_t number);

char* binary_str(u32 number, u8 amount);
