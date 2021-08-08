#include "ppu.h"
#include "scheduler.h"
#include <log.h>

void init_ppu(ppu_t* ppu, scheduler_t* scheduler) {
  memset(ppu->vram, 0, VRAM_SIZE);
  memset(ppu->pram, 0, PRAM_SIZE);
  memset(ppu->oam, 0, OAM_SIZE);
  memset(ppu->framebuffer, 0, GBA_W * GBA_H * 2);
  ppu->frame_finished = false;

  ppu->io.dispcnt.raw = 0x6000;
  ppu->io.dispstat.raw = 0;
  ppu->io.vcount = 227;

  entry_t hdraw_entry = { .event = HDraw, .time = 0 };
  scheduler_push(scheduler, hdraw_entry);
}

u8 read8_io_ppu(ppu_t* ppu, u32 addr) {
  switch(addr & 0xff) {
  case 0x0: return ppu->io.dispcnt.raw & 0xff;
  case 0x1: return ppu->io.dispcnt.raw >> 8;
  case 0x4: return ppu->io.dispstat.raw & 0xff;
  case 0x5: return ppu->io.dispstat.raw >> 8;
  case 0x6: return ppu->io.vcount & 0xff;
  case 0x7: return ppu->io.vcount >> 8;
  case 0x8: return ppu->io.bg0cnt.raw & 0xff;
  case 0x9: return ppu->io.bg0cnt.raw >> 8;
  case 0xA: return ppu->io.bg1cnt.raw & 0xff;
  case 0xB: return ppu->io.bg1cnt.raw >> 8;
  case 0xC: return ppu->io.bg2cnt.raw & 0xff;
  case 0xD: return ppu->io.bg2cnt.raw >> 8;
  case 0xE: return ppu->io.bg3cnt.raw & 0xff;
  case 0xF: return ppu->io.bg3cnt.raw >> 8;
  default: logfatal("Unhandled LCD IO (addr: %08X)!\n", addr);
  }
}

void write8_io_ppu(ppu_t* ppu, u32 addr, u8 val) {
  switch(addr & 0xff) {
  case 0x0: ppu->io.dispcnt.raw = (ppu->io.dispcnt.raw & 0xff00) | val; break;
  case 0x1: ppu->io.dispcnt.raw = ((u16)val << 8) | (ppu->io.dispcnt.raw & 0xff); break;
  case 0x4: ppu->io.dispstat.raw = (ppu->io.dispstat.raw & 0xff00) | val; break;
  case 0x5: ppu->io.dispstat.raw = ((u16)val << 8)  | (ppu->io.dispstat.raw & 0xff); break;
  case 0x8: ppu->io.bg0cnt.raw = (ppu->io.bg0cnt.raw & 0xff00) | val; break;
  case 0x9: ppu->io.bg0cnt.raw = ((u16)val << 8) | (ppu->io.bg0cnt.raw & 0xff); break;
  case 0xA: ppu->io.bg1cnt.raw = (ppu->io.bg1cnt.raw & 0xff00) | val; break;
  case 0xB: ppu->io.bg1cnt.raw = ((u16)val << 8) | (ppu->io.bg1cnt.raw & 0xff); break;
  case 0xC: ppu->io.bg2cnt.raw = (ppu->io.bg2cnt.raw & 0xff00) | val; break;
  case 0xD: ppu->io.bg2cnt.raw = ((u16)val << 8) | (ppu->io.bg2cnt.raw & 0xff); break;
  case 0xE: ppu->io.bg3cnt.raw = (ppu->io.bg3cnt.raw & 0xff00) | val; break;
  case 0xF: ppu->io.bg3cnt.raw = ((u16)val << 8) | (ppu->io.bg3cnt.raw & 0xff); break;
  default: logfatal("Unhandled LCD IO (addr: %08X, val: %02X)!\n", addr, val);
  }
}

u16 read16_io_ppu(ppu_t* ppu, u32 addr) {
  switch(addr & 0xff) {
  case 0x0: return ppu->io.dispcnt.raw;
  case 0x4: return ppu->io.dispstat.raw;
  case 0x6: return ppu->io.vcount;
  case 0x8: return ppu->io.bg0cnt.raw;
  case 0xA: return ppu->io.bg1cnt.raw;
  case 0xC: return ppu->io.bg2cnt.raw;
  case 0xE: return ppu->io.bg3cnt.raw;
  default: logfatal("Unhandled LCD IO (addr: %08X)!\n", addr);
  }
}

void write16_io_ppu(ppu_t* ppu, u32 addr, u16 val) {
  switch(addr & 0xff) {
  case 0x0: ppu->io.dispcnt.raw = val; break;
  case 0x4: ppu->io.dispstat.raw = val; break;
  case 0x8: ppu->io.bg0cnt.raw = val; break;
  case 0xA: ppu->io.bg1cnt.raw = val; break;
  case 0xC: ppu->io.bg2cnt.raw = val; break;
  case 0xE: ppu->io.bg3cnt.raw = val; break;
  default: logfatal("Unhandled LCD IO (addr: %08X, val: %02X)!\n", addr, val);
  }
}

u32 read32_io_ppu(ppu_t* ppu, u32 addr) {
  switch(addr & 0xff) {
  case 0x0: return ppu->io.dispcnt.raw;
  case 0x4: return (((u32)ppu->io.dispstat.raw) << 16) | ppu->io.vcount;
  case 0x8: return (((u32)ppu->io.bg0cnt.raw) << 16) | ppu->io.bg1cnt.raw;
  case 0xC: return (((u32)ppu->io.bg2cnt.raw) << 16) | ppu->io.bg3cnt.raw;
  default: logfatal("Unhandled LCD IO (addr: %08X)!\n", addr);
  }
}

void write32_io_ppu(ppu_t* ppu, u32 addr, u32 val) {
  switch(addr & 0xff) {
  case 0x0:
    ppu->io.dispcnt.raw = val & 0xff;
    // undocumented next 2 bytes do idk what to do 
    break;
  case 0x4:
    ppu->io.dispstat.raw = val & 0xff;
    // VCOUNT is read-only
    break;
  case 0x8:
    ppu->io.bg0cnt.raw = val & 0xff;
    ppu->io.bg1cnt.raw = val >> 8;
    break;
  case 0xC:
    ppu->io.bg2cnt.raw = val & 0xff;
    ppu->io.bg3cnt.raw = val >> 8;
    break;
  default: logfatal("Unhandled LCD IO (addr: %08X, val: %02X)!\n", addr, val);
  }
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
  switch(mode) {
  case 0:
    break;
  case 3:
    mode3(ppu);
    break;
  default:
    logfatal("Unimplemented PPU mode! %d\n", mode);
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
  for(u8 x = 0; x < GBA_W; x++) {
    u8 low_byte = ppu->vram[(GBA_W * ppu->io.vcount + x) << 1];
    u8 high_byte = ppu->vram[(GBA_W * ppu->io.vcount + x) << 1 | 1];
    ppu->framebuffer[GBA_W * ppu->io.vcount + x] = (high_byte << 8) | low_byte;
  }
}