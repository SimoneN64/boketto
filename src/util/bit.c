#include "bit.h"

char* binary_str(u32 number, u8 amount) {
  u8 total = amount + (amount / 4);
  char* result = (char*)malloc(total * sizeof(char));
  memset(result, 0, total);

  for(int j = 0, k = total - 1, i = amount - 1; k >= 0; k--) {
    if(k % 5 == 0) {
      result[j] = '\'';
    } else {
      result[j] = (((number >> i) & 1) == 1) ? '1' : '0';
      i--;
    }

    j++;
  }

  result[total - 1] = '\0';

  return result;
}

s32 sign_extend32(s32 number, u8 amount) {
  return (number << (32 - amount)) >> (32 - amount);
}

size_t next_power_of_two(size_t v) {
  // Taken from "Bit Twiddling Hacks" by Sean Anderson:
  // https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
  --v;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  v |= v >> 16;
  return v + 1;
}
