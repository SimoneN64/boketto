#pragma once
#include <bit.h>
#include <lcd.h>
#include <scheduler.h>
#define VRAM_SIZE 0x18000
#define PRAM_SIZE 0x400
#define OAM_SIZE 0x400
#define VRAM_DSIZE 0x1FFFF
#define PRAM_DSIZE (PRAM_SIZE - 1)
#define OAM_DSIZE (OAM_SIZE - 1)
#define DEPTH 2

typedef struct {
  u8 vram[VRAM_SIZE];
  u8 pram[PRAM_SIZE];
  u8 oam[OAM_SIZE];
  ppu_io_t io;
  u16 framebuffer[GBA_W * GBA_H];
  bool frame_finished;
} ppu_t;

void init_ppu(ppu_t* ppu, scheduler_t* scheduler);
void mode3(ppu_t* ppu);
void mode4(ppu_t* ppu);
void hdraw_dispatch(ppu_t* ppu, u64 time, scheduler_t* scheduler);
void hblank_dispatch(ppu_t* ppu, u64 time, scheduler_t* scheduler);