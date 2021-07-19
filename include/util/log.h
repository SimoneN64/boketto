#pragma once
#include <stdio.h>
#ifdef DEBUG
#define log(fmt, ...) do { printf(fmt, __VA_ARGS__); } while(0)
#endif
#define logfatal(fmt, ...) do { printf(fmt, __VA_ARGS__); exit(1); } while(0)
