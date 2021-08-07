#include "core.h"

void init_core(core_t* core) {
	core->cycles_ran = 0;
	core->running = false;
  init_mem(&core->mem);
	init_cpu(&core->cpu);
  init_scheduler(&core->scheduler);
}

void run_frame(core_t* core) {
	if(core->running) {
		for(int i = 0; i < GBA_CYCLES_PER_FRAME; i++) {
			step_cpu(&core->cpu, &core->mem);
		}
	
		//core->cycles_ran -= GBA_CYCLES_PER_FRAME;
	}
}

void destroy_core(core_t* core) {
	core->running = false;
}