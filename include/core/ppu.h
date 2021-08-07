#pragma once
#include "bit.h"
#include "lcd_io.h"
#define VRAM_SIZE (0x18000)
#define PRAM_SIZE (0x400)
#define OAM_SIZE (0x400)
#define VRAM_DSIZE (VRAM_SIZE - 1)
#define PRAM_DSIZE (PRAM_SIZE - 1)
#define OAM_DSIZE (OAM_SIZE - 1)

typedef struct {
  dispcnt_t dispcnt;
  dispstat_t dispstat;
  bg0cnt_t bg0cnt, bg1cnt, bg2cnt, bg3cnt;
  u16 vcount;
} io_t;

typedef struct {
  u8 vram[0x18000];
  u8 pram[0x400];
  u8 oam[0x400];
  io_t io;
} ppu_t;

void init_ppu(ppu_t* ppu);
void mode3(ppu_t* ppu);
void step_ppu(ppu_t* ppu);
u8 read8_io_ppu(ppu_t* ppu, u32 addr);
void write8_io_ppu(ppu_t* ppu, u32 addr, u8 val);
u16 read16_io_ppu(ppu_t* ppu, u32 addr);
void write16_io_ppu(ppu_t* ppu, u32 addr, u16 val);
u32 read32_io_ppu(ppu_t* ppu, u32 addr);
void write32_io_ppu(ppu_t* ppu, u32 addr, u32 val);