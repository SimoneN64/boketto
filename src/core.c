#include "core.h"
#include <log.h>

void init_core(core_t* core) {
	core->cycles = 0;
	core->running = false;
  init_mem(&core->mem);
	init_cpu(&core->cpu);
  init_scheduler(&core->scheduler);
}

void destroy_core(core_t* core) {
  core->running = false;
}

void run_frame(core_t* core) {
	if(core->running) {
	  infinite_loop {
			step_cpu(&core->cpu, &core->mem);
			core->cycles++;
			if(core->scheduler.entries[0].time < core->cycles) {
			  break;
			}
		}

	  dispatch_events(core);
	}
}

void dispatch_events(core_t* core) {
  int last_pos = 0;
  for(int i = 0; i < core->scheduler.pos; i++) {
    if(core->scheduler.entries[i].time > core->cycles) {
      last_pos = i;
      break;
    }

    switch(core->scheduler.entries[0].event) {
    case None:
      break;
    case HBlank:
      //TODO: dispatch HBlank event
      logfatal("HBlank\n");
    case HDraw:
      //TODO: dispatch HDraw event
      logfatal("HDraw\n");
    case Panic:
      logfatal("Panic event! Achievement unlocked: \"How did we get here?\"\n");
    }
  }

  scheduler_pop(&core->scheduler, last_pos);
}