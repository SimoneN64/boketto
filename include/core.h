#pragma once
#include "cpu.h"
#include "mem.h"
#include "scheduler.h"
#include <nfd.h>

typedef struct {
	u64 cycles;
	bool running;
	nfdchar_t* rom_file;
	cpu_t cpu;
	mem_t mem;
	scheduler_t scheduler;
} core_t;

void init_core(core_t* core);
void run_frame(core_t* core);
void destroy_core(core_t* core);
void dispatch_events(core_t* core);