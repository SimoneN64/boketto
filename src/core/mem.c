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
  FILE* fp = fopen(path, "rb");
  if(fp == NULL) {
    printf("Failed to read rom %s\n", path);
    exit(1);
  }

  fseek(fp, 0, SEEK_END);
  size_t rom_size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  mem->rom = (u8*)malloc(rom_size);
  if(fread(mem->rom, 1, rom_size, fp) != rom_size) {
    printf("Failed to load rom\n");
    exit(1);
  }
  
  fclose(fp);
}

u8 read_8(mem_t* mem, u32 addr) {

}

u16 read_16(mem_t* mem, u32 addr) {

}

u32 read_32(mem_t* mem, u32 addr) {

}

void write_8(mem_t* mem, u32 addr, u8 val) {

}

void write_16(mem_t* mem, u32 addr, u16 val) {

}

void write_32(mem_t* mem, u32 addr, u32 val) {

}
