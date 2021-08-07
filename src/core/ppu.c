#include "ppu.h"
#include <log.h>

void init_ppu(ppu_t* ppu) {
  memset(ppu->vram, 0, VRAM_SIZE);
  memset(ppu->pram, 0, PRAM_SIZE);
  memset(ppu->oam, 0, OAM_SIZE);
  ppu->io.dispcnt.raw = 0;
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

void step_ppu(ppu_t* ppu) {

}

void mode3(ppu_t* ppu) {

}