#include "bit.h"

GET_BIT_RANGE(u8) {
  u8 mask = (UINT8_MAX << end) >> (end - start);
  return (value & mask) >> start;
}

GET_BIT_RANGE(u16) {
  u16 mask = (UINT16_MAX << end) >> (end - start);
  return (value & mask) >> start;
}

GET_BIT_RANGE(u32) {
  u32 mask = (UINT32_MAX << end) >> (end - start);
  return (value & mask) >> start;
}

SET_BIT_RANGE(u8) {
  u8 mask = (UINT8_MAX << end) >> (end - start);
  (*value) |= mask;
}

SET_BIT_RANGE(u16) {
  u16 mask = (UINT16_MAX << end) >> (end - start);
  (*value) |= mask;
}

SET_BIT_RANGE(u32) {
  u32 mask = (UINT32_MAX << end) >> (end - start);
  (*value) |= mask;
}

GET_BIT(u8) {
  return (value >> pos) & 1;
}

GET_BIT(u16) {
  return (value >> pos) & 1;
}

GET_BIT(u32) {
  return (value >> pos) & 1;
}

SET_BIT(u8) {
  (*value) |= (1 << pos);
}

SET_BIT(u16) {
  (*value) |= (1 << pos);
}

SET_BIT(u32) {
  (*value) |= (1 << pos);
}
