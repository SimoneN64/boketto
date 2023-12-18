#include <thumb/instruction/misc.h>
#include <log.h>
#include <helpers.h>

static inline void print_push_pop(u32 instruction) {
#ifdef DEBUG
  static const char* reg_names[9] = {
      "r0", "r1", "r2", "r3",
      "r4", "r5", "r6", "r7",
      "lr"
  };

  u16 list_mask = instruction & 0x1ff;
  bool is_end = false;
  

  for(int i = 0; i < 9; i++) {
    if(((list_mask >> (i + 1)) & (0x1ff >> (i + 1))) == 0) {
      is_end = true;
    }

    if(bit(instruction, i)) {
      
    }
  }

  
#endif
}

THUMB_INSTRUCTION(push) {
  u8 num_reg = 0;
  print_push_pop(registers->instruction);

  for(int i = 0; i < 8; i++) {
    if(bit(registers->instruction, i)) {
      num_reg++;
    }
  }

  u32 start_addr = registers->gpr[13] - (4 * num_reg);
  u32 end_addr = registers->gpr[13] - 4;
  registers->gpr[13] -= 4 * num_reg;
  for(int i = 0; i < 9 && start_addr < end_addr; start_addr += 4, i++) {
    if(bit(registers->instruction, i)) {
      if(i == 8) {
        write_32(mem, registers->gpr[PC], start_addr, registers->gpr[13]);
        continue;
      }
      write_32(mem, registers->gpr[PC], start_addr, registers->gpr[i]);
    }
  }
}

THUMB_INSTRUCTION(pop) {
  u8 num_reg = 0;
  print_push_pop(registers->instruction);

  for(int i = 0; i < 8; i++) {
    if(bit(registers->instruction, i)) {
      num_reg++;
    }
  }

  u32 start_addr = registers->gpr[13];
  u32 end_addr = (registers->gpr[13] + (4 * num_reg)) - 4;
  registers->gpr[13] += 4 * num_reg;
  for(int i = 0; i < 9 && start_addr < end_addr; start_addr += 4, i++) {
    if(bit(registers->instruction, i)) {
      if(i == 8) {
        set_pc(false, mem, registers, read_32(mem, registers->gpr[PC], start_addr) & 0xfffffffe, registers->cpsr.thumb);
        continue;
      }
      registers->gpr[i] = read_32(mem, registers->gpr[PC], start_addr);
    }
  }
}

THUMB_INSTRUCTION(asr_imm) {
  u8 rd = registers->instruction & 7;
  u8 rm = bits(registers->instruction,3, 5);
  u8 imm = bits(registers->instruction, 6, 10);
  
  if(imm == 0) {
    registers->cpsr.carry = registers->gpr[rm] >> 31;
    registers->gpr[rd] = (s32)registers->gpr[rm] >> 31;
  } else {
    registers->cpsr.carry = bit(registers->gpr[rm], imm - 1);
    registers->gpr[rd] = (s32)registers->gpr[rm] >> imm;
  }

  registers->cpsr.negative = registers->gpr[rd] >> 31;
  registers->cpsr.zero = registers->gpr[rd] == 0;
}