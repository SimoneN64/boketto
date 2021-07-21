#include "mem.h"
#include <memory.h>
#include <stdio.h>

void init_mem(mem_t* mem) {
  memset(mem->bios, 0, BIOS_SIZE);
  memset(mem->iWRAM, 0, IWRAM_SIZE);
  memset(mem->eWRAM, 0, EWRAM_SIZE);
  memset(mem->io, 0, IO_SIZE);
}

void load_rom(mem_t* mem, const char* path) {
  log("Loading rom: %s\n", path);
  FILE* fp = fopen(path, "rb");
  if(fp == NULL) {
    logfatal("Failed to open rom %s\n", path);
  }

  fseek(fp, 0, SEEK_END);
  size_t rom_size = ftell(fp);
  printf("Rom size: %ld\n", rom_size);
  mem->rom_size = rom_size;
  fseek(fp, 0, SEEK_SET);

  mem->rom = (u8*)malloc(rom_size);
  if(fread(mem->rom, 1, rom_size, fp) != rom_size) {
    logfatal("Failed to load rom\n");
  }

  fclose(fp);
}

u8 read_8(mem_t* mem, u32 addr) {
  switch(addr) {
    case 0x00000000 ... 0x00003FFF:
      return mem->bios[addr];
    case 0x00004000 ... 0x01FFFFFF:
      log("[WARN][MEM] Open bus read! (%08X)\n", addr);
      return 0xff;
    case 0x02000000 ... 0x0203FFFF:
      return mem->eWRAM[addr & EWRAM_DSIZE];
    case 0x03000000 ... 0x03007FFF:
      return mem->iWRAM[addr & IWRAM_DSIZE];
    case 0x04000000 ... 0x040003FE:
      return mem->io[addr & IO_DSIZE];
    case 0x08000000 ... 0x0DFFFFFF:
      return mem->rom[addr & (mem->rom_size - 1)];
    default:
      logfatal("[ERR ][MEM] Unhandled address! (%08X)\n", addr);
  }
}

u16 read_16(mem_t* mem, u32 addr) {
  return ((u16)read_8(mem, addr + 1) << 8) | read_8(mem, addr);
}

u32 read_32(mem_t* mem, u32 addr) {
  return ((u32)read_16(mem, addr + 2) << 16) | read_16(mem, addr);
}

void write_8(mem_t* mem, u32 addr, u8 val) {
  switch(addr) {
  case 0x00004000 ... 0x01FFFFFF:
    log("[WARN][MEM] Open bus write! (%08X, %02X)\n", addr, val);
    break;
  case 0x02000000 ... 0x0203FFFF:
    mem->eWRAM[addr & EWRAM_DSIZE] = val;
    break;
  case 0x03000000 ... 0x03007FFF:
    mem->iWRAM[addr & IWRAM_DSIZE] = val;
    break;
  case 0x04000000 ... 0x040003FE:
    mem->io[addr & IO_DSIZE] = val;
    break;
  default:
    logfatal("[ERR ][MEM] Unhandled address! (%08X, %02X)\n", addr, val);
  }
}

void write_16(mem_t* mem, u32 addr, u16 val) {
  write_8(mem, addr + 1, val >> 8);
  write_8(mem, addr, val);
}

void write_32(mem_t* mem, u32 addr, u32 val) {
  write_16(mem, addr + 2, val >> 16);
  write_16(mem, addr, val);
}
