#include "scheduler.h"
#include <string.h>

void init_scheduler(scheduler_t* scheduler) {
  scheduler->entries[0].time = UINT64_MAX;
  scheduler->entries[0].event = Panic;
  scheduler->pos = 1;

  for(int i = scheduler->pos; i < ENTRIES_MAX; i++) {
    scheduler->entries[i].time = 0;
    scheduler->entries[i].event = None;
  }
}

void scheduler_push(scheduler_t* scheduler, entry_t entry) {
  if(scheduler->pos < ENTRIES_MAX) {
    for(int i = 0; i < scheduler->pos; i++) {
      if(scheduler->entries[i].time > entry.time) {
        memmove(&scheduler->entries[i + 1], &scheduler->entries[i], (scheduler->pos - i) * sizeof(entry_t));
        scheduler->entries[i] = entry;
        break;
      }
    }

    scheduler->pos++;
  }
}

void scheduler_pop(scheduler_t* scheduler, int count) {
  memmove(&scheduler->entries[0], &scheduler->entries[count], sizeof(entry_t) * (scheduler->pos - count));
  scheduler->pos -= count;
}
