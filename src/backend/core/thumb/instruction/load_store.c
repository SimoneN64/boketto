#include <thumb/instruction/load_store.h>
#include <log.h>
#include <helpers.h>

THUMB_INSTRUCTION(unimplemented_load_store) {
  logfatal("Unimplemented thumb load/store instruction %04X (%s)\n", instr, binary_str(instr, 16));
}

THUMB_INSTRUCTION(ldr_reg) {
  u8 rd = instr & 7;
  u8 rn = (instr >> 3) & 7;
  u8 imm = bits(instr, 6, 10);
  u32 addr = (imm * 4) + registers->gpr[rn];
  addr &= ~3;
  
  registers->gpr[rd] = read_32(mem, registers->gpr[PC], addr);
}

THUMB_INSTRUCTION(str_reg) {
  u8 rd = instr & 7;
  u8 rn = (instr >> 3) & 7;
  u8 imm = bits(instr, 6, 10);
  u32 addr = (imm * 4) + registers->gpr[rn];
  addr &= ~3;
  
  write_32(mem, registers->gpr[PC], addr, registers->gpr[rd]);
}

THUMB_INSTRUCTION(strh_imm) {
  u8 rd = instr & 7;
  u8 rn = (instr >> 3) & 7;
  u8 imm = bits(instr, 6, 10);
  
  u32 addr = registers->gpr[rn] + (imm * 2);
  addr &= ~1;
  write_16(mem, registers->gpr[PC], addr, registers->gpr[rd]);
}

THUMB_INSTRUCTION(ldr_pc) {
  u8 rd = bits(instr, 8, 10);
  u16 imm = (instr & 0xff) << 2;
  u32 addr = (registers->gpr[PC] & ~3) + imm;
  
  registers->gpr[rd] = read_32(mem, registers->gpr[PC], addr);
}

THUMB_INSTRUCTION(stmia) {
  u8 rn = (instr >> 8) & 7;
  u32 base_address = registers->gpr[rn];

  u8 list_mask = instr & 0xff;

  for(u8 i = 0; i < 8; i++) {
    if(bit(list_mask, i)) {
      write_32(mem, registers->gpr[PC], base_address, registers->gpr[i]);
      base_address += 4;
    }
  }

  registers->gpr[rn] = base_address;
}

THUMB_INSTRUCTION(ldmia) {
  u8 rb = (instr >> 8) & 7;
  u32 base_address = registers->gpr[rb];

  u8 list_mask = instr & 0xff;

  for(u8 i = 0; i < 8; i++) {
    if(bit(list_mask, i)) {
      registers->gpr[i] = read_32(mem, registers->gpr[PC], base_address);
      base_address += 4;
    }
  }

  registers->gpr[rb] = base_address;
}