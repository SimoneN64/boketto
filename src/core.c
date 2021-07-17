#include "core.h"

void init_core(core_t* core) {
	core->cycles_ran = 0;
	core->running = false;
	init_cpu(&core->cpu);
	init_mem(&core->mem);
}

void run_frame(core_t* core) {
	while(core->running && (core->cycles_ran < GBA_CYCLES_PER_FRAME)) {
		step_cpu(&core->cpu, &core->mem);
	}
	
	if(core->running)
		core->cycles_ran -= GBA_CYCLES_PER_FRAME;
}

void destroy_core(core_t* core) {
	core->running = false;
}