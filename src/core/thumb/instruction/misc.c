#include <thumb/instruction/misc.h>
#include <log.h>

THUMB_INSTRUCTION(push) {
  u8 num_reg = 0;
  u16 list_mask = registers->instruction & 0x1ff;
#ifdef DEBUG
  static const char* reg_names[9] = {
      "r0", "r1", "r2", "r3",
      "r4", "r5", "r6", "r7",
      "lr"
  };

  bool is_end = false;
  logdebug("push { ");
#endif
  for(int i = 0; i < 9; i++) {
#ifdef DEBUG
    if(((list_mask >> (i + 1)) & (0x1ff >> (i + 1))) == 0) {
      is_end = true;
    }
#endif
    if(bit(registers->instruction, i)) {
      num_reg++;
#ifdef DEBUG
      logdebug(is_end ? "%s" : "%s, ", reg_names[i]);
#endif
    }
  }

  logdebug(" }\n");

  u32 start_addr = registers->gpr[13] - (4 * num_reg);
  u32 end_addr = registers->gpr[13] - 4;
  registers->gpr[13] -= 4 * num_reg;
  for(int i = 0; i < 9 && start_addr < end_addr; start_addr += 4, i++) {
    if(bit(registers->instruction, i)) {
      if(i == 8) {
        write_32(mem, start_addr, registers->gpr[13]);
        continue;
      }
      write_32(mem, start_addr, registers->gpr[i]);
    }
  }
}