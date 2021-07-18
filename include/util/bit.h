#pragma once
#include "common.h"
#define mask(len) (1 << ((len) - 1))
#define bits(n, lo, hi) (((n) >> (lo)) & mask((hi) - (lo)))
#define bit(n, pos) (((n) >> (pos)) & 1)
#define sex(n, amt) ((n) << ((sizeof(n) * 8) - (amt)) >> ((sizeof(n) * 8) - (amt)))