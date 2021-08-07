#pragma once
#include "bit.h"
#define VRAM_SIZE (0x18000)
#define PRAM_SIZE (0x400)
#define OAM_SIZE (0x400)
#define VRAM_DSIZE (VRAM_SIZE - 1)
#define PRAM_DSIZE (PRAM_SIZE - 1)
#define OAM_DSIZE (OAM_SIZE - 1)

typedef struct {
  u8 vram[0x18000];
  u8 pram[0x400];
  u8 oam[0x400];
} ppu_t;

void init_ppu(ppu_t* ppu);