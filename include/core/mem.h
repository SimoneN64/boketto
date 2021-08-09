#pragma once
#include "ppu.h"
#define BIOS_SIZE 0x4000
#define EWRAM_SIZE 0x40000
#define IWRAM_SIZE 0x8000
#define EWRAM_DSIZE (EWRAM_SIZE - 1)
#define IWRAM_DSIZE (IWRAM_SIZE - 1)

typedef struct {
  u8 bios[BIOS_SIZE];
  u8 iWRAM[IWRAM_SIZE];
  u8 eWRAM[EWRAM_SIZE];
  u8* rom;
  size_t rom_size;
  ppu_t ppu;
  bool ime;
} mem_t;

void init_mem(mem_t* mem);
void load_rom(mem_t* mem, const char* path);

u8  read_8 (mem_t* mem, u32 addr);
u16 read_16(mem_t* mem, u32 addr);
u32 read_32(mem_t* mem, u32 addr);

void write_8 (mem_t* mem, u32 addr, u8 val);
void write_16(mem_t* mem, u32 addr, u16 val);
void write_32(mem_t* mem, u32 addr, u32 val);