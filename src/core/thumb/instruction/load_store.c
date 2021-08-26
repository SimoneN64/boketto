#include <thumb/instruction/load_store.h>
#include <log.h>
#include <helpers.h>

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

THUMB_INSTRUCTION(stmia) {
  u8 rn = (registers->instruction >> 8) & 7;
  u32 base_address = registers->gpr[rn];

  u8 list_mask = registers->instruction & 0xff;
  print_list(true, registers->instruction);

  assert(list_mask != 0);

  for(u8 i = 0; i < 8; i++) {
    if(bit(list_mask, i)) {
      write_32(mem, base_address, registers->gpr[i]);
      base_address += 4;
    }
  }

  registers->gpr[rn] = base_address;
}

THUMB_INSTRUCTION(ldmia) {
  u8 rb = (registers->instruction >> 8) & 7;
  u32 base_address = registers->gpr[rb];

  u8 list_mask = registers->instruction & 0xff;
  print_list(true, registers->instruction);

  assert(list_mask != 0);

  for(u8 i = 0; i < 8; i++) {
    if(bit(list_mask, i)) {
      registers->gpr[i] = read_32(mem, base_address);
      base_address += 4;
    }
  }

  registers->gpr[rb] = base_address;
}