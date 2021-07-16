#pragma once
#include "common.h"
#define GET_BIT_RANGE(type) type get_bit_range_##type(type value, u8 start, u8 end)
#define SET_BIT_RANGE(type) void set_bit_range_##type(type* value, u8 start, u8 end)
#define GET_BIT(type) type get_bit_##type(type value, u8 pos)
#define SET_BIT(type) void set_bit_##type(type* value, u8 pos)

GET_BIT_RANGE(u8);
GET_BIT_RANGE(u16);
GET_BIT_RANGE(u32);

SET_BIT_RANGE(u8);
SET_BIT_RANGE(u16);
SET_BIT_RANGE(u32);

GET_BIT(u8);
GET_BIT(u16);
GET_BIT(u32);

SET_BIT(u8);
SET_BIT(u16);
SET_BIT(u32);