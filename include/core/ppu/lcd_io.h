#pragma once
#include "common.h"

typedef union {
  struct {
    unsigned obj_win:1;
    unsigned win1:1;
    unsigned win0:1;
    unsigned screen_obj:1;
    unsigned screen_bg3:1;
    unsigned screen_bg2:1;
    unsigned screen_bg1:1;
    unsigned screen_bg0:1;
    unsigned force_blank:1;
    unsigned vram_mapping:1;
    unsigned oam_access_hblank:1;
    unsigned frame_select:1;
    unsigned cgb_mode:1;
    unsigned bg_mode:3;
  };

  u16 raw;
} dispcnt_t;

typedef union {
  struct {
    unsigned lyc:8;
    unsigned:2;
    unsigned vc_irq:1;
    unsigned hb_irq:1;
    unsigned vb_irq:1;
    unsigned vc:1;
    unsigned hb:1;
    unsigned vb:1;
  };

  u16 raw;
} dispstat_t;

typedef union {
  struct {
    unsigned size:2;
    unsigned disp_area_overflow:1;
    unsigned base_blk:5;
    unsigned colors:1;
    unsigned mosaic:1;
    unsigned:2;
    unsigned char_base_blk:2;
    unsigned bg_prio:2;
  };

  u16 raw;
} bg0cnt_t;