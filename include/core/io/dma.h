#pragma once
#include <common.h>

typedef struct mem_t mem_t;

typedef union {
  struct {
    unsigned:5;
    unsigned dest_ctrl:2;
    unsigned src_ctrl:2;
    unsigned repeat:1;
    unsigned type:1;
    unsigned drq:1;
    unsigned start_timing:2;
    unsigned irq:1;
    unsigned enable:1;
  };

  u16 raw;
} dmacnt_h_t;

typedef enum {
  IMMEDIATE,
  VBLANK,
  HBLANK,
  SOUND,
  VIDEO_CAPTURE,
  NONE,
} dma_start_timing_t;

typedef struct {
  u32 sad, dad, current_sad, current_dad;
  u16 unit_count, remaining_units, unit_count_mask;
  dmacnt_h_t control;
  s8 sad_increment, dad_increment;
  dma_start_timing_t start_timing;
} dma_channel_t;

typedef struct {
  dma_channel_t channels[4];
  s8 active_channel;
} dmac_t;

void set_dma_channel_control(mem_t* mem, u8 i, u16 val);

void init_dma(dmac_t* dmac);

u8 read8_io_dma(mem_t* mem, u32 addr);
u16 read16_io_dma(mem_t* mem, u32 addr);
u32 read32_io_dma(mem_t* mem, u32 addr);
void write8_io_dma(mem_t* mem, u32 addr, u8 val);
void write16_io_dma(mem_t* mem, u32 addr, u16 val);
void write32_io_dma(mem_t* mem, u32 addr, u32 val);