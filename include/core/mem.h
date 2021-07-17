#pragma once
#include "bit.h"
#define BIOS_SIZE 0x4000
#define IWRAM_SIZE 0x40000
#define EWRAM_SIZE 0x8000
#define IO_SIZE 0x3FF

typedef struct {
  u8 bios[0x4000];
  u8 iWRAM[0x40000];
  u8 eWRAM[0x8000];
  u8 io[0x3ff];
  u8* rom;
} mem_t;

void init_mem(mem_t* mem);
void load_rom(mem_t* mem, const char* path);

u8  read_8 (mem_t* mem, u32 addr);
u16 read_16(mem_t* mem, u32 addr);
u32 read_32(mem_t* mem, u32 addr);

void write_8 (mem_t* mem, u32 addr, u8 val);
void write_16(mem_t* mem, u32 addr, u16 val);
void write_32(mem_t* mem, u32 addr, u32 val);