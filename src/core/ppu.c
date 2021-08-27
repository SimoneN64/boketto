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
  }
}

void write8_io_ppu(ppu_t* ppu, u32 addr, u8 val) {
  switch(addr & 0xff) {
  case 0x00: ppu->io.dispcnt.raw = (ppu->io.dispcnt.raw & 0xff00) | val; break;
  case 0x01: ppu->io.dispcnt.raw = ((u16)val << 8) | (ppu->io.dispcnt.raw & 0xff); break;
  case 0x04: ppu->io.dispstat.raw = (ppu->io.dispstat.raw & 0xff00) | val; break;
  case 0x05: ppu->io.dispstat.raw = ((u16)val << 8)  | (ppu->io.dispstat.raw & 0xff); break;
  case 0x08: ppu->io.bg0cnt.raw = (ppu->io.bg0cnt.raw & 0xff00) | val; break;
  case 0x09: ppu->io.bg0cnt.raw = ((u16)val << 8) | (ppu->io.bg0cnt.raw & 0xff); break;
  case 0x0A: ppu->io.bg1cnt.raw = (ppu->io.bg1cnt.raw & 0xff00) | val; break;
  case 0x0B: ppu->io.bg1cnt.raw = ((u16)val << 8) | (ppu->io.bg1cnt.raw & 0xff); break;
  case 0x0C: ppu->io.bg2cnt.raw = (ppu->io.bg2cnt.raw & 0xff00) | val; break;
  case 0x0D: ppu->io.bg2cnt.raw = ((u16)val << 8) | (ppu->io.bg2cnt.raw & 0xff); break;
  case 0x0E: ppu->io.bg3cnt.raw = (ppu->io.bg3cnt.raw & 0xff00) | val; break;
  case 0x0F: ppu->io.bg3cnt.raw = ((u16)val << 8) | (ppu->io.bg3cnt.raw & 0xff); break;
  case 0x10: ppu->io.bg0hofs = (ppu->io.bg0cnt.raw & 0xff00) | val; break;
  case 0x11: ppu->io.bg0hofs = ((u16)val << 8) | (ppu->io.bg0cnt.raw & 0xff); break;
  case 0x12: ppu->io.bg0vofs.raw = (ppu->io.bg1cnt.raw & 0xff00) | val; break;
  case 0x13: ppu->io.bg0vofs.raw = ((u16)val << 8) | (ppu->io.bg1cnt.raw & 0xff); break;
  case 0x14: ppu->io.bg1hofs = (ppu->io.bg2cnt.raw & 0xff00) | val; break;
  case 0x15: ppu->io.bg1hofs = ((u16)val << 8) | (ppu->io.bg2cnt.raw & 0xff); break;
  case 0x16: ppu->io.bg1vofs.raw = (ppu->io.bg3cnt.raw & 0xff00) | val; break;
  case 0x17: ppu->io.bg1vofs.raw = ((u16)val << 8) | (ppu->io.bg3cnt.raw & 0xff); break;
  case 0x18: ppu->io.bg2hofs = (ppu->io.bg0cnt.raw & 0xff00) | val; break;
  case 0x19: ppu->io.bg2hofs = ((u16)val << 8) | (ppu->io.bg0cnt.raw & 0xff); break;
  case 0x1A: ppu->io.bg2vofs.raw = (ppu->io.bg1cnt.raw & 0xff00) | val; break;
  case 0x1B: ppu->io.bg2vofs.raw = ((u16)val << 8) | (ppu->io.bg1cnt.raw & 0xff); break;
  case 0x1C: ppu->io.bg3hofs = (ppu->io.bg2cnt.raw & 0xff00) | val; break;
  case 0x1D: ppu->io.bg3hofs = ((u16)val << 8) | (ppu->io.bg2cnt.raw & 0xff); break;
  case 0x1E: ppu->io.bg3vofs.raw = (ppu->io.bg3cnt.raw & 0xff00) | val; break;
  case 0x1F: ppu->io.bg3vofs.raw = ((u16)val << 8) | (ppu->io.bg3cnt.raw & 0xff); break;
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
  }
}

void write16_io_ppu(ppu_t* ppu, u32 addr, u16 val) {
  switch(addr & 0xff) {
  case 0x00: ppu->io.dispcnt.raw = val; break;
  case 0x04: ppu->io.dispstat.raw = val; break;
  case 0x08: ppu->io.bg0cnt.raw = val; break;
  case 0x0A: ppu->io.bg1cnt.raw = val; break;
  case 0x0C: ppu->io.bg2cnt.raw = val; break;
  case 0x0E: ppu->io.bg3cnt.raw = val; break;
  case 0x10: ppu->io.bg0hofs = val; break;
  case 0x12: ppu->io.bg0vofs.raw = val; break;
  case 0x14: ppu->io.bg1hofs = val; break;
  case 0x16: ppu->io.bg1vofs.raw = val; break;
  case 0x18: ppu->io.bg2hofs = val; break;
  case 0x1A: ppu->io.bg2vofs.raw = val; break;
  case 0x1C: ppu->io.bg3hofs = val; break;
  case 0x1E: ppu->io.bg3cnt.raw = val; break;
  }
}

u32 read32_io_ppu(ppu_t* ppu, u32 addr) {
  switch(addr & 0xff) {
  case 0x0: return ppu->io.dispcnt.raw;
  case 0x4: return (((u32)ppu->io.dispstat.raw) << 16) | ppu->io.vcount;
  case 0x8: return (((u32)ppu->io.bg0cnt.raw) << 16) | ppu->io.bg1cnt.raw;
  case 0xC: return (((u32)ppu->io.bg2cnt.raw) << 16) | ppu->io.bg3cnt.raw;
  }
}

void write32_io_ppu(ppu_t* ppu, u32 addr, u32 val) {
  switch(addr & 0xff) {
  case 0x00:
    ppu->io.dispcnt.raw = val & 0xffff;
    // undocumented next 2 bytes do idk what to do 
    break;
  case 0x04:
    ppu->io.dispstat.raw = val & 0xffff;
    // VCOUNT is read-only
    break;
  case 0x08:
    ppu->io.bg0cnt.raw = val & 0xffff;
    ppu->io.bg1cnt.raw = val >> 16;
    break;
  case 0x0C:
    ppu->io.bg2cnt.raw = val & 0xffff;
    ppu->io.bg3cnt.raw = val >> 16;
    break;
  case 0x10:
    ppu->io.bg0hofs = val & 0xffff;
    ppu->io.bg0vofs.raw = val >> 16;
    break;
  case 0x14:
    ppu->io.bg1hofs = val & 0xffff;
    ppu->io.bg1vofs.raw = val >> 16;
    break;
  case 0x18:
    ppu->io.bg2hofs = val & 0xffff;
    ppu->io.bg2vofs.raw = val >> 16;
    break;
  case 0x1C:
    ppu->io.bg3hofs = val & 0xffff;
    ppu->io.bg3vofs.raw = val >> 16;
    break;
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
  u32 bufferIndex = vramIndex * DEPTH;
  for(int x = 0; x < GBA_W; x++) {
    const u32 paletteIndex = ppu->vram[vramIndex] * 2;
    ppu->framebuffer[bufferIndex] = *(u16*)&ppu->pram[paletteIndex];

    ++vramIndex;
    ++bufferIndex;
  }
}