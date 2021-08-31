#include <ppu.h>
#include <scheduler.h>
#include <log.h>

void init_ppu(ppu_t* ppu, scheduler_t* scheduler) {
  memset(ppu->vram, 0, VRAM_SIZE);
  memset(ppu->pram, 0, PRAM_SIZE);
  memset(ppu->oam, 0, OAM_SIZE);
  memset(ppu->framebuffer, 0, GBA_W * GBA_H * DEPTH);
  ppu->frame_finished = false;

  ppu->io.dispcnt.raw = 0x6000;
  ppu->io.dispstat.raw = 0;
  ppu->io.vcount = 227;

  entry_t hdraw_entry = { .event = HDraw, .time = 0 };
  scheduler_push(scheduler, hdraw_entry);
}

void hdraw_dispatch(ppu_t* ppu, const u64 time, scheduler_t* scheduler) {
  ppu->io.dispstat.hb = 0;
  ppu->io.vcount++;

  switch (ppu->io.vcount) {
  case 160:
    ppu->io.dispstat.vb = 1;
    break;
  case 228:
    ppu->frame_finished = true;
    ppu->io.vcount = 0;
    ppu->io.dispstat.vb = 0;
    break;
  }
  
  u8 mode = ppu->io.dispcnt.bg_mode;
  if (ppu->io.vcount < GBA_H) {
    switch(mode) {
    case 0:
      break;
    case 3:
      mode3(ppu);
      break;
    case 4:
      mode4(ppu);
      break;
    default:
      logfatal("Unimplemented PPU mode! %d\n", mode);
    }
  }

  entry_t hblank_entry = { .event = HBlank, .time = time + 960 };
  scheduler_push(scheduler, hblank_entry);
}

void hblank_dispatch(ppu_t* ppu, const u64 time, scheduler_t* scheduler) {
  ppu->io.dispstat.hb = 1;
  entry_t hblank_entry = { .event = HDraw, .time = time + 272 };
  scheduler_push(scheduler, hblank_entry);
}

void mode3(ppu_t* ppu) {
  u32 bufferIndex = ppu->io.vcount * GBA_W * DEPTH;

  for(int x = 0; x < GBA_W; x++) {
    ppu->framebuffer[bufferIndex >> 1] = *(u16*)&ppu->vram[bufferIndex];
    bufferIndex += DEPTH;
  }
}

void mode4(ppu_t* ppu) {
  u32 vramIndex = ppu->io.vcount * GBA_W;
  u32 bufferIndex = vramIndex;
  for(int x = 0; x < GBA_W; x++) {
    const u32 paletteIndex = ppu->vram[vramIndex] * 2;
    ppu->framebuffer[bufferIndex] = *(u16*)&ppu->pram[paletteIndex];

    ++vramIndex;
    ++bufferIndex;
  }
}