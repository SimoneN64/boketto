#include "mem.h"
#include <stdio.h>
#include "log.h"

void init_mem(mem_t* mem) {
  memset(mem->bios, 0, BIOS_SIZE);
  memset(mem->iWRAM, 0, IWRAM_SIZE);
  memset(mem->eWRAM, 0, EWRAM_SIZE);
}

void load_rom(mem_t* mem, const char* path) {
  logdebug("Loading rom: %s\n", path);
  FILE* fp = fopen(path, "rb");
  if(fp == NULL) {
    logfatal("Failed to open rom %s\n", path);
  }

  fseek(fp, 0, SEEK_END);
  size_t rom_size = ftell(fp);
  size_t rounded_rom_size = next_power_of_two(rom_size);
  logdebug("Rom size: %zu (rounded to %zu)\n", rom_size, rounded_rom_size);
  mem->rom_size = rounded_rom_size;
  fseek(fp, 0, SEEK_SET);

  mem->rom = (u8*)malloc(rounded_rom_size);
  if(fread(mem->rom, 1, rom_size, fp) != rom_size) {
    logfatal("Failed to load rom\n");
  }
  memset(&mem->rom[rom_size], 0, rounded_rom_size - rom_size);

  fclose(fp);
}

static const inline char* region_str(u32 addr) {
  switch(addr) {
  case 0x00000000 ... 0x00003FFF:
    return "BIOS";
  case 0x02000000 ... 0x02FFFFFF:
    return "eWRAM";
  case 0x03000000 ... 0x03FFFFFF:
    return "iWRAM";
  case 0x04000000 ... 0x040003FE:
    return "IO";
  case 0x05000000 ... 0x05FFFFFF:
    return "PRAM";
  case 0x06000000 ... 0x06FFFFFF:
    return "VRAM";
  case 0x07000000 ... 0x07FFFFFF:
    return "OAM";
  case 0x08000000 ... 0x0DFFFFFF:
    return "ROM";
  case 0x0E000000 ... 0x0FFFFFFF:
    return "SRAM";
  default:
    return "Open Bus";
  }
}

u8 read_8(mem_t* mem, u32 pc, u32 addr) {
  u8 val;

  switch(addr) {
  case 0x00000000 ... 0x00003FFF:
    val = mem->bios[addr];
    break;
  case 0x00004000 ... 0x01FFFFFF:
    logdebug("[INFO][MEM] Read from Open Bus (STUB!) (%08X)\n", addr);
    return 0xff;
  case 0x02000000 ... 0x02FFFFFF:
    val = mem->eWRAM[addr & EWRAM_DSIZE];
    break;
  case 0x03000000 ... 0x03FFFFFF:
    val = mem->iWRAM[addr & IWRAM_DSIZE];
    break;
  case 0x04000000 ... 0x04000058:
    val = read8_io_ppu(&mem->ppu.io, addr);
    break;
  case 0x040000B0 ... 0x040000DF:
    val = read8_io_dma(mem, addr);
    break;
  case 0x04000208:
    val = mem->ime;
    break;
  case 0x05000000 ... 0x05FFFFFF:
    val = mem->ppu.pram[addr & PRAM_DSIZE];
    break;
  case 0x06000000 ... 0x06FFFFFF:
    val = mem->ppu.vram[addr & VRAM_DSIZE];
    break;
  case 0x07000000 ... 0x07FFFFFF:
    val = mem->ppu.oam[addr & OAM_DSIZE];
    break;
  case 0x08000000 ... 0x0DFFFFFF:
    val = mem->rom[addr & (mem->rom_size - 1)];
    break;
  default:
    logfatal("[ERR ][MEM] Read from unhandled %s! (%08X) (PC: %08X)\n", region_str(addr), addr, pc);
  }

  logdebug("[INFO][MEM] Read from %s (%08X)\n", region_str(addr), addr);
  return val;
}

u16 read_16(mem_t* mem, u32 pc, u32 addr) {
  u16 val;
  assert((addr & 1) == 0);

  switch(addr) {
  case 0x00000000 ... 0x00003FFF:
    val = *(u16*)&mem->bios[addr];
    break;
  case 0x00004000 ... 0x01FFFFFF:
    logdebug("[INFO][MEM] Read from Open Bus (STUB!) (%08X)\n", addr);
    return 0xffff;
  case 0x02000000 ... 0x02FFFFFF:
    val = *(u16*)&mem->eWRAM[addr & EWRAM_DSIZE];
    break;
  case 0x03000000 ... 0x03FFFFFF:
    val = *(u16*)&mem->iWRAM[addr & IWRAM_DSIZE];
    break;
  case 0x04000000 ... 0x04000058:
    val = read16_io_ppu(&mem->ppu.io, addr);
    break;
  case 0x040000B0 ... 0x040000DF:
    val = read16_io_dma(mem, addr);
    break;
  case 0x04000208:
    val = mem->ime;
    break;
  case 0x05000000 ... 0x05FFFFFF:
    val = *(u16*)&mem->ppu.pram[addr & PRAM_DSIZE];
    break;
  case 0x06000000 ... 0x06FFFFFF:
    val = *(u16*)&mem->ppu.vram[addr & VRAM_DSIZE];
    break;
  case 0x07000000 ... 0x07FFFFFF:
    val = *(u16*)&mem->ppu.oam[addr & OAM_DSIZE];
    break;
  case 0x08000000 ... 0x0DFFFFFF:
    val = *(u16*)&mem->rom[addr & (mem->rom_size - 1)];
    break;
  default:
    logfatal("[ERR ][MEM] Read from unhandled %s! (%08X) (PC: %08X)\n", region_str(addr), addr, pc);
  }

  logdebug("[INFO][MEM] Read from %s (%08X)\n", region_str(addr), addr);
  return val;
}

u32 read_32(mem_t* mem, u32 pc, u32 addr) {
  u32 val;
  assert((addr & 3) == 0);

  switch(addr) {
  case 0x00000000 ... 0x00003FFF:
    val = *(u32*)&mem->bios[addr];
    break;
  case 0x00004000 ... 0x01FFFFFF:
    logdebug("[INFO][MEM] Read from Open Bus (STUB!) (%08X)\n", addr);
    return 0xffffffff;
  case 0x02000000 ... 0x02FFFFFF:
    val = *(u32*)&mem->eWRAM[addr & EWRAM_DSIZE];
    break;
  case 0x03000000 ... 0x03FFFFFF:
    val = *(u32*)&mem->iWRAM[addr & IWRAM_DSIZE];
    break;
  case 0x04000000 ... 0x04000058:
    val = read32_io_ppu(&mem->ppu.io, addr);
    break;
  case 0x040000B0 ... 0x040000DF:
    val = read32_io_dma(mem, addr);
    break;
  case 0x04000208:
    val = mem->ime;
    break;
  case 0x05000000 ... 0x05FFFFFF:
    val = *(u32*)&mem->ppu.pram[addr & PRAM_DSIZE];
    break;
  case 0x06000000 ... 0x06FFFFFF:
    val = *(u32*)&mem->ppu.vram[addr & VRAM_DSIZE];
    break;
  case 0x07000000 ... 0x07FFFFFF:
    val = *(u32*)&mem->ppu.oam[addr & OAM_DSIZE];
    break;
  case 0x08000000 ... 0x0DFFFFFF:
    val = *(u32*)&mem->rom[addr & (mem->rom_size - 1)];
    break;
  default:
    logfatal("[WARN][MEM] Read from unhandled %s! (%08X) (PC: %08X)\n", region_str(addr), addr, pc);
  }

  logdebug("[INFO][MEM] Read from %s (%08X)\n", region_str(addr), addr);
  return val;
}

void write_8(mem_t* mem, u32 pc, u32 addr, u8 val) {
  switch(addr) {
  case 0x02000000 ... 0x02FFFFFF:
    mem->eWRAM[addr & EWRAM_DSIZE] = val;
    break;
  case 0x03000000 ... 0x03FFFFFF:
    mem->iWRAM[addr & IWRAM_DSIZE] = val;
    break;
  case 0x04000000 ... 0x04000058:
    write8_io_ppu(&mem->ppu.io, addr, val);
    break;
  case 0x040000B0 ... 0x040000DF:
    write8_io_dma(mem, addr, val);
    break;
  case 0x04000208:
    mem->ime = val;
    break;
  case 0x05000000 ... 0x05FFFFFF:
    mem->ppu.pram[addr & PRAM_DSIZE] = val;
    break;
  case 0x06000000 ... 0x06FFFFFF:
    mem->ppu.vram[addr & VRAM_DSIZE] = val;
    break;
  case 0x07000000 ... 0x07FFFFFF:
    mem->ppu.oam[addr & OAM_DSIZE] = val;
    break;
  default:
    logfatal("[ERR ][MEM] Write (%02X) to unhandled %s (%08X) (PC: %08X)!\n", val, region_str(addr), addr, pc);
  }
  logdebug("[INFO][MEM] Write (%02X) to %s (%08X)\n", val, region_str(addr), addr);
}

void write_16(mem_t* mem, u32 pc, u32 addr, u16 val) {
  assert((addr & 1) == 0);

  switch(addr) {
  case 0x00004000 ... 0x01FFFFFF:
    break;
  case 0x02000000 ... 0x02FFFFFF:
    *(u16*)&mem->eWRAM[addr & EWRAM_DSIZE] = val;
    break;
  case 0x03000000 ... 0x03FFFFFF:
    *(u16*)&mem->iWRAM[addr & IWRAM_DSIZE] = val;
    break;
  case 0x04000000 ... 0x04000058:
    write16_io_ppu(&mem->ppu.io, addr, val);
    break;
  case 0x040000B0 ... 0x040000DF:
    write16_io_dma(mem, addr, val);
    break;
  case 0x04000208:
    mem->ime = val;
    break;
  case 0x05000000 ... 0x05FFFFFF:
    *(u16*)&mem->ppu.pram[addr & PRAM_DSIZE] = val;
    break;
  case 0x06000000 ... 0x06FFFFFF:
    *(u16*)&mem->ppu.vram[addr & VRAM_DSIZE] = val;
    break;
  case 0x07000000 ... 0x07FFFFFF:
    *(u16*)&mem->ppu.oam[addr & OAM_DSIZE] = val;
    break;
  default:
    logfatal("[ERR ][MEM] Write (%04X) to unhandled %s (%08X) (PC: %08X)!\n", val, region_str(addr), addr, pc);
  }

  logdebug("[INFO][MEM] Write (%04X) to %s (%08X)\n", val, region_str(addr), addr);
}

void write_32(mem_t* mem, u32 pc, u32 addr, u32 val) {
  assert((addr & 3) == 0);

  switch(addr) {
  case 0x00004000 ... 0x01FFFFFF:
    break;
  case 0x02000000 ... 0x02FFFFFF:
    *(u32*)&mem->eWRAM[addr & EWRAM_DSIZE] = val;
    break;
  case 0x03000000 ... 0x03FFFFFF:
    *(u32*)&mem->iWRAM[addr & IWRAM_DSIZE] = val;
    break;
  case 0x04000000 ... 0x04000058:
    write32_io_ppu(&mem->ppu.io, addr, val);
    break;
  case 0x040000B0 ... 0x040000DF:
    write32_io_dma(mem, addr, val);
    break;
  case 0x04000208:
    mem->ime = val;
    break;
  case 0x05000000 ... 0x05FFFFFF:
    *(u32*)&mem->ppu.pram[addr & PRAM_DSIZE] = val;
    break;
  case 0x06000000 ... 0x06FFFFFF:
    *(u32*)&mem->ppu.vram[addr & VRAM_DSIZE] = val;
    break;
  case 0x07000000 ... 0x07FFFFFF:
    *(u32*)&mem->ppu.oam[addr & OAM_DSIZE] = val;
    break;
  default:
    logfatal("[WARN][MEM] Write (%08X) to unhandled %s (%08X) (PC: %08X)!\n", val, region_str(addr), addr, pc);
  }
  logdebug("[INFO][MEM] Write (%08X) to %s (%08X)\n", val, region_str(addr), addr);
}
