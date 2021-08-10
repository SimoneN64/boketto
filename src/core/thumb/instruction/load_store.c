#include "thumb/instruction/load_store.h"
#include "log.h"

THUMB_INSTRUCTION(unimplemented_load_store) {
  logfatal("Unimplemented thumb load/store instruction %04X (%s)\n", registers->instruction, binary_str(registers->instruction, 16));
}

THUMB_INSTRUCTION(ldr) {
  u8 rd = bits(registers->instruction, 8, 10);
  u16 imm = (registers->instruction & 0xff) << 2;
  u32 addr = (registers->gpr[PC] & ~3) + imm;
  logdebug("ldr r%d, [PC, %04X (%08X)]\n", rd, imm, addr);
  registers->gpr[rd] = read_32(mem, addr);
}