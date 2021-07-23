#include "bit.h"

char* binary_str(u32 number) {
  char* result = (char*)malloc(40 * sizeof(char));
  memset(result, 0, 40);

  for(int j = 0, k = 39, i = (sizeof(u32) * 8) - 1; k >= 0; k--) {
    if(k % 5 == 0) {
      result[j] = '\'';
    } else {
      result[j] = (((number >> i) & 1) == 1) ? '1' : '0';
      i--;
    }

    j++;
  }

  result[39] = '\0';

  return result;
}

s32 sex(s32 number, u8 amount) {
  return (number << (32 - amount)) >> (32 - amount);
}