#pragma once
#include <bit.h>
#include <lcd.h>
#include <scheduler.h>
#include <stdatomic.h>

typedef struct {
  u8 vram[VRAM_SIZE];
  u8 pram[PRAM_SIZE];
  u8 oam[OAM_SIZE];
  ppu_io_t io;
  atomic_uint_fast8_t current_framebuffer;
  u16 framebuffers[2][GBA_W * GBA_H];
  bool frame_finished;
} ppu_t;

void init_ppu(ppu_t* ppu, scheduler_t* scheduler);
void mode3(ppu_t* ppu);
void mode4(ppu_t* ppu);
void hdraw_dispatch(ppu_t* ppu, u64 time, scheduler_t* scheduler);
void hblank_dispatch(ppu_t* ppu, u64 time, scheduler_t* scheduler);