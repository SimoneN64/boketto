#pragma once
#include "common.h"
#define ENTRIES_MAX 32

typedef enum {
  None,
  HBlank,
  HDraw,
  Panic,
} event_t;

typedef struct {
  event_t event;
  u64 time;
} entry_t;

typedef struct {
  entry_t entries[ENTRIES_MAX];
  int pos;
} scheduler_t;

void init_scheduler(scheduler_t* scheduler);
void scheduler_push(scheduler_t* scheduler, entry_t entry);
void scheduler_pop(scheduler_t* scheduler, int count);