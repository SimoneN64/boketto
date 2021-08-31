#pragma once
#include <common.h>

typedef union {
  struct {
    unsigned bg_mode:3;
    unsigned cgb_mode:1;
    unsigned frame_select:1;
    unsigned oam_access_hblank:1;
    unsigned vram_mapping:1;
    unsigned force_blank:1;
    unsigned screen_bg0:1;
    unsigned screen_bg1:1;
    unsigned screen_bg2:1;
    unsigned screen_bg3:1;
    unsigned screen_obj:1;
    unsigned win0:1;
    unsigned win1:1;
    unsigned obj_win:1;
  };

  u16 raw;
} dispcnt_t;

typedef union {
  struct {
    unsigned vb:1;
    unsigned hb:1;
    unsigned vc:1;
    unsigned vb_irq:1;
    unsigned hb_irq:1;
    unsigned vc_irq:1;
    unsigned:2;
    unsigned lyc:8;
  };

  u16 raw;
} dispstat_t;

typedef union {
  struct {
    unsigned bg_prio:2;
    unsigned char_base_blk:2;
    unsigned:2;
    unsigned mosaic:1;
    unsigned colors:1;
    unsigned base_blk:5;
    unsigned disp_area_overflow:1;
    unsigned size:2;
  };

  u16 raw;
} bgcnt_t;

typedef union {
  struct {
    unsigned offset:9;
    unsigned:7;
  };

  u16 raw;
} bgvofs_t;

typedef struct {
  dispcnt_t dispcnt;
  dispstat_t dispstat;
  bgcnt_t bg0cnt, bg1cnt, bg2cnt, bg3cnt;
  bgvofs_t bg0vofs, bg1vofs, bg2vofs, bg3vofs;
  u16 bg0hofs, bg1hofs, bg2hofs, bg3hofs;
  u16 vcount;
} ppu_io_t;

u8 read8_io_ppu(ppu_io_t* io, u32 addr);
u16 read16_io_ppu(ppu_io_t* io, u32 addr);
u32 read32_io_ppu(ppu_io_t* io, u32 addr);
void write8_io_ppu(ppu_io_t* io, u32 addr, u8 val);
void write16_io_ppu(ppu_io_t* io, u32 addr, u16 val);
void write32_io_ppu(ppu_io_t* io, u32 addr, u32 val);