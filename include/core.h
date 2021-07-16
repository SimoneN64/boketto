#pragma once
#include "bit.h"
#include <nfd.h>

typedef struct {
	u64 cycles_ran;
	bool running;
	nfdchar_t* rom_file;
} core_t;

void init_core(core_t* core);
void load_rom(core_t* core);
void run_frame(core_t* core);
void destroy_core(core_t* core);