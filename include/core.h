#pragma once
#include <nfd.h>
#include <cpu.h>
#include <mem.h>
#include <scheduler.h>

typedef struct {
	u64 cycles;
	bool running;
	cpu_t cpu;
	mem_t mem;
	scheduler_t scheduler;
} core_t;

void init_core(core_t* core);
void dispatch_events(core_t* core);
void run_frame(core_t* core);