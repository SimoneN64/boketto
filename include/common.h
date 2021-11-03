#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define BIOS_SIZE 0x4000
#define EWRAM_SIZE 0x40000
#define IWRAM_SIZE 0x8000
#define EWRAM_DSIZE (EWRAM_SIZE - 1)
#define IWRAM_DSIZE (IWRAM_SIZE - 1)
#define VRAM_SIZE 0x18000
#define PRAM_SIZE 0x400
#define OAM_SIZE 0x400
#define VRAM_DSIZE 0x1FFFF
#define PRAM_DSIZE (PRAM_SIZE - 1)
#define OAM_DSIZE (OAM_SIZE - 1)
#define DEPTH 2

#define GBA_W 240
#define GBA_H 160

typedef  uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef  int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;