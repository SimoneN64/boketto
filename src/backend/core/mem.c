#include "mem.h"
#include <stdio.h>
#include "log.h"

void init_mem(mem_t* mem, scheduler_t* scheduler) {
  memset(mem->bios, 0, BIOS_SIZE);
  memset(mem->iWRAM, 0, IWRAM_SIZE);
  memset(mem->eWRAM, 0, EWRAM_SIZE);
  init_ppu(&mem->ppu, scheduler);
  init_dma(&mem->dmac);
}

bool load_rom(mem_t* mem, const char* path) {
  FILE* fp = fopen(path, "rb");
  if(fp == NULL) {
    printf("Failed to open rom %s\n", path);
    return false;
  }

  fseek(fp, 0, SEEK_END);
  size_t rom_size = ftell(fp);
  size_t rounded_rom_size = next_power_of_two(rom_size);
  
  mem->rom_size = rounded_rom_size;
  fseek(fp, 0, SEEK_SET);

  mem->rom = (u8*)calloc(rounded_rom_size, 1);
  if(fread(mem->rom, 1, rom_size, fp) != rom_size) {
    printf("Failed to load rom\n");
    return false;
  }

  fclose(fp);
  return true;
}

static const inline char* region_str(u32 addr) {
  switch(addr) {
  case 0x00000000 ... 0x00003FFF: return "BIOS";
  case 0x02000000 ... 0x02FFFFFF: return "eWRAM";
  case 0x03000000 ... 0x03FFFFFF: return "iWRAM";
  case 0x04000000 ... 0x040003FE: return "IO";
  case 0x05000000 ... 0x05FFFFFF: return "PRAM";
  case 0x06000000 ... 0x06FFFFFF: return "VRAM";
  case 0x07000000 ... 0x07FFFFFF: return "OAM";
  case 0x08000000 ... 0x0DFFFFFF: return "ROM";
  case 0x0E000000 ... 0x0FFFFFFF: return "SRAM";
  default: return "Open Bus";
  }
}

u8 read_8(mem_t* mem, u32 pc, u32 addr) {
  switch(addr) {
  case 0x00000000 ... 0x00003FFF: return mem->bios[addr];    
  case 0x00004000 ... 0x01FFFFFF: return 0xff;
  case 0x02000000 ... 0x02FFFFFF: return mem->eWRAM[addr & EWRAM_DSIZE];    
  case 0x03000000 ... 0x03FFFFFF: return mem->iWRAM[addr & IWRAM_DSIZE];    
  case 0x04000000 ... 0x04000058: return read8_io_ppu(&mem->ppu.io, addr);    
  case 0x040000B0 ... 0x040000DF: return read8_io_dma(mem, addr);    
  case 0x04000208: return mem->ime;    
  case 0x05000000 ... 0x05FFFFFF: return mem->ppu.pram[addr & PRAM_DSIZE];    
  case 0x06000000 ... 0x06FFFFFF: return mem->ppu.vram[addr & VRAM_DSIZE];    
  case 0x07000000 ... 0x07FFFFFF: return mem->ppu.oam[addr & OAM_DSIZE];    
  case 0x08000000 ... 0x0DFFFFFF: return mem->rom[addr & (mem->rom_size - 1)];    
  default: break;
    logfatal("[ERR][MEM] Read8 from unhandled %s! (%08X) (PC: %08X)\n", region_str(addr), addr, pc);
  }
}

u16 read_16(mem_t* mem, u32 pc, u32 addr) {
  assert((addr & 1) == 0);

  switch(addr) {
  case 0x00000000 ... 0x00003FFF: return *(u16*)&mem->bios[addr];    
  case 0x00004000 ... 0x01FFFFFF: return 0xffff;
  case 0x02000000 ... 0x02FFFFFF: return *(u16*)&mem->eWRAM[addr & EWRAM_DSIZE];    
  case 0x03000000 ... 0x03FFFFFF: return *(u16*)&mem->iWRAM[addr & IWRAM_DSIZE];    
  case 0x04000000 ... 0x04000058: return read16_io_ppu(&mem->ppu.io, addr);    
  case 0x040000B0 ... 0x040000DF: return read16_io_dma(mem, addr);
  case 0x04000208: return mem->ime;
  case 0x05000000 ... 0x05FFFFFF: return *(u16*)&mem->ppu.pram[addr & PRAM_DSIZE];
  case 0x06000000 ... 0x06FFFFFF: return *(u16*)&mem->ppu.vram[addr & VRAM_DSIZE];
  case 0x07000000 ... 0x07FFFFFF: return *(u16*)&mem->ppu.oam[addr & OAM_DSIZE]; 
  case 0x08000000 ... 0x0DFFFFFF: return *(u16*)&mem->rom[addr & (mem->rom_size - 1)];
  default: break;
   logfatal("[ERR][MEM] Read16 from unhandled %s! (%08X) (PC: %08X)\n", region_str(addr), addr, pc);
  }
}

u32 read_32(mem_t* mem, u32 pc, u32 addr) {
  assert((addr & 3) == 0);

  switch(addr) {
  case 0x00000000 ... 0x00003FFF: return *(u32*)&mem->bios[addr];
  case 0x00004000 ... 0x01FFFFFF: return 0xffffffff;
  case 0x02000000 ... 0x02FFFFFF: return *(u32*)&mem->eWRAM[addr & EWRAM_DSIZE];
  case 0x03000000 ... 0x03FFFFFF: return *(u32*)&mem->iWRAM[addr & IWRAM_DSIZE];
  case 0x04000000 ... 0x04000058: return read32_io_ppu(&mem->ppu.io, addr);
  case 0x040000B0 ... 0x040000DF: return read32_io_dma(mem, addr);
  case 0x04000208: return mem->ime;
  case 0x05000000 ... 0x05FFFFFF: return *(u32*)&mem->ppu.pram[addr & PRAM_DSIZE];
  case 0x06000000 ... 0x06FFFFFF: return *(u32*)&mem->ppu.vram[addr & VRAM_DSIZE];
  case 0x07000000 ... 0x07FFFFFF: return *(u32*)&mem->ppu.oam[addr & OAM_DSIZE];
  case 0x08000000 ... 0x0DFFFFFF: return *(u32*)&mem->rom[addr & (mem->rom_size - 1)];
  default:
    logfatal("[ERR][MEM] Read32 from unhandled %s! (%08X) (PC: %08X)\n", region_str(addr), addr, pc);
  }
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
  default: break;
    logfatal("[ERR][MEM] Write8 (%02X) to unhandled %s (%08X) (PC: %08X)!\n", val, region_str(addr), addr, pc);
  }
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
  default: break;
//    logfatal("[ERR][MEM] Write16 (%04X) to unhandled %s (%08X) (PC: %08X)!\n", val, region_str(addr), addr, pc);
  }
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
  default: break;
//    logfatal("[ERR][MEM] Write32 (%08X) to unhandled %s (%08X) (PC: %08X)!\n", val, region_str(addr), addr, pc);
  } 
}
