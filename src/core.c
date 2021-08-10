#include "core.h"
#include <log.h>

void init_core(core_t* core) {
	core->cycles = 0;
	core->running = false;
  init_scheduler(&core->scheduler);
  init_mem(&core->mem);
  init_ppu(&core->mem.ppu, &core->scheduler);
	init_cpu(&core->cpu);
}

void destroy_core(core_t* core) {
  core->running = false;
}

void run_frame(core_t* core) {
  core->mem.ppu.frame_finished = false;
  if(core->running) {
    while(!core->mem.ppu.frame_finished) {
      while(core->cycles < core->scheduler.entries[0].time) {
        step_cpu(&core->cpu, &core->mem);
        core->cycles++;
      }

      dispatch_events(core);
    }
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
      hblank_dispatch(&core->mem.ppu, core->scheduler.entries[i].time, &core->scheduler);
      break;
    case HDraw:
      hdraw_dispatch(&core->mem.ppu, core->scheduler.entries[i].time, &core->scheduler);
      break; 
    case Panic:
      logfatal("Panic event! Achievement unlocked: \"How did we get here?\"\n");
    }
  }

  scheduler_pop(&core->scheduler, last_pos);
}