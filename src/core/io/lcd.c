#include <lcd.h>

u8 read8_io_ppu(ppu_io_t* io, u32 addr) {
  switch(addr & 0xff) {
    case 0x0: return io->dispcnt.raw & 0xff;
    case 0x1: return io->dispcnt.raw >> 8;
    case 0x4: return io->dispstat.raw & 0xff;
    case 0x5: return io->dispstat.raw >> 8;
    case 0x6: return io->vcount & 0xff;
    case 0x7: return io->vcount >> 8;
    case 0x8: return io->bg0cnt.raw & 0xff;
    case 0x9: return io->bg0cnt.raw >> 8;
    case 0xA: return io->bg1cnt.raw & 0xff;
    case 0xB: return io->bg1cnt.raw >> 8;
    case 0xC: return io->bg2cnt.raw & 0xff;
    case 0xD: return io->bg2cnt.raw >> 8;
    case 0xE: return io->bg3cnt.raw & 0xff;
    case 0xF: return io->bg3cnt.raw >> 8;
  }
}

u16 read16_io_ppu(ppu_io_t* io, u32 addr) {
  switch(addr & 0xff) {
    case 0x0: return io->dispcnt.raw;
    case 0x4: return io->dispstat.raw;
    case 0x6: return io->vcount;
    case 0x8: return io->bg0cnt.raw;
    case 0xA: return io->bg1cnt.raw;
    case 0xC: return io->bg2cnt.raw;
    case 0xE: return io->bg3cnt.raw;
  }
}

u32 read32_io_ppu(ppu_io_t* io, u32 addr) {
  switch(addr & 0xff) {
    case 0x0: return io->dispcnt.raw;
    case 0x4: return io->vcount     | (((u32)io->dispstat.raw) << 16);
    case 0x8: return io->bg1cnt.raw | (((u32)io->bg0cnt.raw) << 16);
    case 0xC: return io->bg3cnt.raw | (((u32)io->bg2cnt.raw) << 16);
  }
}

void write8_io_ppu(ppu_io_t* io, u32 addr, u8 val) {
  switch(addr & 0xff) {
    case 0x00: io->dispcnt.raw  = (io->dispcnt.raw & 0xff00) | val; break;
    case 0x01: io->dispcnt.raw  = (io->dispcnt.raw & 0x00ff) | ((u16)val << 8); break;
    case 0x04: io->dispstat.raw = (io->dispstat.raw & 0xff00) | val; break;
    case 0x05: io->dispstat.raw = (io->dispstat.raw & 0x00ff) | ((u16)val << 8); break;
    case 0x08: io->bg0cnt.raw   = (io->bg0cnt.raw & 0xff00) | val; break;
    case 0x09: io->bg0cnt.raw   = (io->bg0cnt.raw & 0x00ff) | ((u16)val << 8); break;
    case 0x0A: io->bg1cnt.raw   = (io->bg1cnt.raw & 0xff00) | val; break;
    case 0x0B: io->bg1cnt.raw   = (io->bg1cnt.raw & 0x00ff) | ((u16)val << 8); break;
    case 0x0C: io->bg2cnt.raw   = (io->bg2cnt.raw & 0xff00) | val; break;
    case 0x0D: io->bg2cnt.raw   = (io->bg2cnt.raw & 0x00ff) | ((u16)val << 8); break;
    case 0x0E: io->bg3cnt.raw   = (io->bg3cnt.raw & 0xff00) | val; break;
    case 0x0F: io->bg3cnt.raw   = (io->bg3cnt.raw & 0x00ff) | ((u16)val << 8); break;
    case 0x10: io->bg0hofs      = (io->bg0cnt.raw & 0xff00) | val; break;
    case 0x11: io->bg0hofs      = (io->bg0cnt.raw & 0x00ff) | ((u16)val << 8); break;
    case 0x12: io->bg0vofs.raw  = (io->bg1cnt.raw & 0xff00) | val; break;
    case 0x13: io->bg0vofs.raw  = (io->bg1cnt.raw & 0x00ff) | ((u16)val << 8); break;
    case 0x14: io->bg1hofs      = (io->bg2cnt.raw & 0xff00) | val; break;
    case 0x15: io->bg1hofs      = (io->bg2cnt.raw & 0x00ff) | ((u16)val << 8); break;
    case 0x16: io->bg1vofs.raw  = (io->bg3cnt.raw & 0xff00) | val; break;
    case 0x17: io->bg1vofs.raw  = (io->bg3cnt.raw & 0x00ff) | ((u16)val << 8); break;
    case 0x18: io->bg2hofs      = (io->bg0cnt.raw & 0xff00) | val; break;
    case 0x19: io->bg2hofs      = (io->bg0cnt.raw & 0x00ff) | ((u16)val << 8); break;
    case 0x1A: io->bg2vofs.raw  = (io->bg1cnt.raw & 0xff00) | val; break;
    case 0x1B: io->bg2vofs.raw  = (io->bg1cnt.raw & 0x00ff) | ((u16)val << 8); break;
    case 0x1C: io->bg3hofs      = (io->bg2cnt.raw & 0xff00) | val; break;
    case 0x1D: io->bg3hofs      = (io->bg2cnt.raw & 0x00ff) | ((u16)val << 8); break;
    case 0x1E: io->bg3vofs.raw  = (io->bg3cnt.raw & 0xff00) | val; break;
    case 0x1F: io->bg3vofs.raw  = (io->bg3cnt.raw & 0x00ff) | ((u16)val << 8); break;
  }
}

void write16_io_ppu(ppu_io_t* io, u32 addr, u16 val) {
  switch(addr & 0xff) {
    case 0x00: io->dispcnt.raw = val; break;
    case 0x04: io->dispstat.raw = val; break;
    case 0x08: io->bg0cnt.raw = val; break;
    case 0x0A: io->bg1cnt.raw = val; break;
    case 0x0C: io->bg2cnt.raw = val; break;
    case 0x0E: io->bg3cnt.raw = val; break;
    case 0x10: io->bg0hofs = val; break;
    case 0x12: io->bg0vofs.raw = val; break;
    case 0x14: io->bg1hofs = val; break;
    case 0x16: io->bg1vofs.raw = val; break;
    case 0x18: io->bg2hofs = val; break;
    case 0x1A: io->bg2vofs.raw = val; break;
    case 0x1C: io->bg3hofs = val; break;
    case 0x1E: io->bg3cnt.raw = val; break;
  }
}

void write32_io_ppu(ppu_io_t* io, u32 addr, u32 val) {
  switch(addr & 0xff) {
    case 0x00:
      io->dispcnt.raw = val & 0xffff;
      // undocumented next 2 bytes do idk what to do
      break;
    case 0x04:
      io->dispstat.raw = val & 0xffff;
      // VCOUNT is read-only
      break;
    case 0x08:
      io->bg0cnt.raw = val & 0xffff;
      io->bg1cnt.raw = val >> 16;
      break;
    case 0x0C:
      io->bg2cnt.raw = val & 0xffff;
      io->bg3cnt.raw = val >> 16;
      break;
    case 0x10:
      io->bg0hofs = val & 0xffff;
      io->bg0vofs.raw = val >> 16;
      break;
    case 0x14:
      io->bg1hofs = val & 0xffff;
      io->bg1vofs.raw = val >> 16;
      break;
    case 0x18:
      io->bg2hofs = val & 0xffff;
      io->bg2vofs.raw = val >> 16;
      break;
    case 0x1C:
      io->bg3hofs = val & 0xffff;
      io->bg3vofs.raw = val >> 16;
      break;
  }
}
