#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define GBA_W 240
#define GBA_H 160
#define GBA_CYCLES_PER_FRAME 16777216 / 60

typedef  uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef  int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;