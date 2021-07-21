#include "mem.h"
#include <stdio.h>
#include "log.h"

void init_mem(mem_t* mem) {
  memset(mem->bios, 0, BIOS_SIZE);
  memset(mem->iWRAM, 0, IWRAM_SIZE);
  memset(mem->eWRAM, 0, EWRAM_SIZE);
  memset(mem->io, 0, IO_SIZE);
}

void load_rom(mem_t* mem, const char* path) {
  logdebug("Loading rom: %s\n", path);
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
      logdebug("[INFO][MEM] Read from BIOS (%08X)\n", addr);
      return mem->bios[addr];
    case 0x00004000 ... 0x01FFFFFF:
      logdebug("[WARN][MEM] Open bus read! (%08X)\n", addr);
      return 0xff;
    case 0x02000000 ... 0x0203FFFF:
      logdebug("[INFO][MEM] Read from eWRAM (%08X)\n", addr);
      return mem->eWRAM[addr & EWRAM_DSIZE];
    case 0x03000000 ... 0x03007FFF:
      logdebug("[INFO][MEM] Read from iWRAM (%08X)\n", addr);
      return mem->iWRAM[addr & IWRAM_DSIZE];
    case 0x04000000 ... 0x040003FE:
      logdebug("[INFO][MEM] Read from IO (%08X)\n", addr);
      return mem->io[addr & IO_DSIZE];
    case 0x08000000 ... 0x0DFFFFFF:
      logdebug("[INFO][MEM] Read from ROM (%08X)\n", addr);
      return mem->rom[addr & (mem->rom_size - 1)];
    default:
      logfatal("[ERR ][MEM] Unhandled address! (%08X)\n", addr);
  }
}

u16 read_16(mem_t* mem, u32 addr) {
  switch(addr) {
    case 0x00000000 ... 0x00003FFF:
      logdebug("[INFO][MEM] Read from BIOS (%08X)\n", addr);
      return *(u16*)&mem->bios[addr];
    case 0x00004000 ... 0x01FFFFFF:
      logdebug("[WARN][MEM] Open bus read! (%08X)\n", addr);
      return 0xff;
    case 0x02000000 ... 0x0203FFFF:
      logdebug("[INFO][MEM] Read from eWRAM (%08X)\n", addr);
      return *(u16*)&mem->eWRAM[addr & EWRAM_DSIZE];
    case 0x03000000 ... 0x03007FFF:
      logdebug("[INFO][MEM] Read from iWRAM (%08X)\n", addr);
      return *(u16*)&mem->iWRAM[addr & IWRAM_DSIZE];
    case 0x04000000 ... 0x040003FE:
      logdebug("[INFO][MEM] Read from IO (%08X)\n", addr);
      return *(u16*)&mem->io[addr & IO_DSIZE];
    case 0x08000000 ... 0x0DFFFFFF:
      logdebug("[INFO][MEM] Read from ROM (%08X)\n", addr);
      return *(u16*)&mem->rom[addr & (mem->rom_size - 1)];
    default:
      logfatal("[ERR ][MEM] Unhandled address! (%08X)\n", addr);
  }
}

u32 read_32(mem_t* mem, u32 addr) {
  switch(addr) {
    case 0x00000000 ... 0x00003FFF:
      logdebug("[INFO][MEM] Read from BIOS (%08X)\n", addr);
      return *(u32*)&mem->bios[addr];
    case 0x00004000 ... 0x01FFFFFF:
      logdebug("[WARN][MEM] Open bus read! (%08X)\n", addr);
      return 0xff;
    case 0x02000000 ... 0x0203FFFF:
      logdebug("[INFO][MEM] Read from eWRAM (%08X)\n", addr);
      return *(u32*)&mem->eWRAM[addr & EWRAM_DSIZE];
    case 0x03000000 ... 0x03007FFF:
      logdebug("[INFO][MEM] Read from iWRAM (%08X)\n", addr);
      return *(u32*)&mem->iWRAM[addr & IWRAM_DSIZE];
    case 0x04000000 ... 0x040003FE:
      logdebug("[INFO][MEM] Read from IO (%08X)\n", addr);
      return *(u32*)&mem->io[addr & IO_DSIZE];
    case 0x08000000 ... 0x0DFFFFFF:
      logdebug("[INFO][MEM] Read from ROM (%08X)\n", addr);
      return *(u32*)&mem->rom[addr & (mem->rom_size - 1)];
    default:
      logfatal("[ERR ][MEM] Unhandled address! (%08X)\n", addr);
  }
}

void write_8(mem_t* mem, u32 addr, u8 val) {
  switch(addr) {
  case 0x00004000 ... 0x01FFFFFF:
    logdebug("[WARN][MEM] Open bus write! (%08X, %02X)\n", addr, val);
    break;
  case 0x02000000 ... 0x0203FFFF:
    logdebug("[INFO][MEM] Write (%02X) to eWRAM (%08X)\n", val, addr);
    mem->eWRAM[addr & EWRAM_DSIZE] = val;
    break;
  case 0x03000000 ... 0x03007FFF:
    logdebug("[INFO][MEM] Write (%02X) to iWRAM (%08X)\n", val, addr);
    mem->iWRAM[addr & IWRAM_DSIZE] = val;
    break;
  case 0x04000000 ... 0x040003FE:
    logdebug("[INFO][MEM] Write (%02X) to IO (%08X)\t[WARN] STUB!\n", val, addr);
    mem->io[addr & IO_DSIZE] = val;
    break;
  default:
    logfatal("[ERR ][MEM] Unhandled address! (%08X, %02X)\n", addr, val);
  }
}

void write_16(mem_t* mem, u32 addr, u16 val) {
  switch(addr) {
    case 0x00004000 ... 0x01FFFFFF:
      logdebug("[WARN][MEM] Open bus write! (%08X, %02X)\n", addr, val);
      break;
    case 0x02000000 ... 0x0203FFFF:
      logdebug("[INFO][MEM] Write (%02X) to eWRAM (%08X)\n", val, addr);
      *(u16*)&mem->eWRAM[addr & EWRAM_DSIZE] = val;
      break;
    case 0x03000000 ... 0x03007FFF:
      logdebug("[INFO][MEM] Write (%02X) to iWRAM (%08X)\n", val, addr);
      *(u16*)&mem->iWRAM[addr & IWRAM_DSIZE] = val;
      break;
    case 0x04000000 ... 0x040003FE:
      logdebug("[INFO][MEM] Write (%02X) to IO (%08X)\t[WARN] STUB!\n", val, addr);
      *(u16*)&mem->io[addr & IO_DSIZE] = val;
      break;
    default:
      logfatal("[ERR ][MEM] Unhandled address! (%08X, %02X)\n", addr, val);
  }
}

void write_32(mem_t* mem, u32 addr, u32 val) {
  switch(addr) {
    case 0x00004000 ... 0x01FFFFFF:
      logdebug("[WARN][MEM] Open bus write! (%08X, %02X)\n", addr, val);
      break;
    case 0x02000000 ... 0x0203FFFF:
      logdebug("[INFO][MEM] Write (%02X) to eWRAM (%08X)\n", val, addr);
      *(u32*)&mem->eWRAM[addr & EWRAM_DSIZE] = val;
      break;
    case 0x03000000 ... 0x03007FFF:
      logdebug("[INFO][MEM] Write (%02X) to iWRAM (%08X)\n", val, addr);
      *(u32*)&mem->iWRAM[addr & IWRAM_DSIZE] = val;
      break;
    case 0x04000000 ... 0x040003FE:
      logdebug("[INFO][MEM] Write (%02X) to IO (%08X)\t[WARN] STUB!\n", val, addr);
      *(u32*)&mem->io[addr & IO_DSIZE] = val;
      break;
    default:
      logfatal("[ERR ][MEM] Unhandled address! (%08X, %02X)\n", addr, val);
  }
}
