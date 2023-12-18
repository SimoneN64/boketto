#pragma once
#include <ppu.h>
#include <io/dma.h>
#include <scheduler/scheduler.h>

typedef struct mem_t {
  u8 bios[BIOS_SIZE];
  u8 iWRAM[IWRAM_SIZE];
  u8 eWRAM[EWRAM_SIZE];
  u8* rom;
  size_t rom_size;
  ppu_t ppu;
  dmac_t dmac;
  bool ime;
} mem_t;

struct registers_t;

void init_mem(mem_t* mem, scheduler_t* scheduler);
bool load_rom(mem_t* mem, const char* path);

u8  read_8 (mem_t* mem, u32 pc, u32 addr);
u16 read_16(mem_t* mem, u32 pc, u32 addr);
u32 read_32(mem_t* mem, u32 pc, u32 addr);

void write_8 (mem_t* mem, u32 pc, u32 addr, u8 val);
void write_16(mem_t* mem, u32 pc, u32 addr, u16 val);
void write_32(mem_t* mem, u32 pc, u32 addr, u32 val);