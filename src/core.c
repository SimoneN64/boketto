#include "core.h"

void init_core(core_t* core) {
	core = (core_t*)malloc(sizeof(core_t));
	if(core == NULL) {
		printf("Failed to allocate core struct\n");
		exit(-1);
	}

	core->cycles_ran = 0;
	core->running = false;
}

void run_frame(core_t* core) {
	//while(core->running && (core->cycles_ran < GBA_CYCLES_PER_FRAME)) {
	//
	//}
	//
	//if(core->running)
	//	core->cycles_ran -= GBA_CYCLES_PER_FRAME;
}

void load_rom(core_t* core) {

}

void destroy_core(core_t* core) {
	core->running = false;
}