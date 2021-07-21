#include "data_processing.h"
#include "log.h"

void arm_handle_data_processing(registers_t* regs, mem_t* mem) {
  switch(bits(regs->instruction, 21, 24)) {
    case 0b0000: logfatal("Unhandled AND (%08X)(%s)", regs->instruction, binary_str(regs->instruction));
    case 0b0001: logfatal("Unhandled EOR (%08X)(%s)", regs->instruction, binary_str(regs->instruction));
    case 0b0010: logfatal("Unhandled SUB (%08X)(%s)", regs->instruction, binary_str(regs->instruction));
    case 0b0011: logfatal("Unhandled RSB (%08X)(%s)", regs->instruction, binary_str(regs->instruction));
    case 0b0100: logfatal("Unhandled ADD (%08X)(%s)", regs->instruction, binary_str(regs->instruction));
    case 0b0101: logfatal("Unhandled ADC (%08X)(%s)", regs->instruction, binary_str(regs->instruction));
    case 0b0110: logfatal("Unhandled SBC (%08X)(%s)", regs->instruction, binary_str(regs->instruction));
    case 0b0111: logfatal("Unhandled RSC (%08X)(%s)", regs->instruction, binary_str(regs->instruction));
    case 0b1000: logfatal("Unhandled TST (%08X)(%s)", regs->instruction, binary_str(regs->instruction));
    case 0b1001: logfatal("Unhandled TEQ (%08X)(%s)", regs->instruction, binary_str(regs->instruction));
    case 0b1010: logfatal("Unhandled CMP (%08X)(%s)", regs->instruction, binary_str(regs->instruction));
    case 0b1011: logfatal("Unhandled CMN (%08X)(%s)", regs->instruction, binary_str(regs->instruction));
    case 0b1100: logfatal("Unhandled ORR (%08X)(%s)", regs->instruction, binary_str(regs->instruction));
    case 0b1101: logfatal("Unhandled MOV (%08X)(%s)", regs->instruction, binary_str(regs->instruction));
    case 0b1110: logfatal("Unhandled BIC (%08X)(%s)", regs->instruction, binary_str(regs->instruction));
    case 0b1111: logfatal("Unhandled MVN (%08X)(%s)", regs->instruction, binary_str(regs->instruction));
  }
}

ARM_INSTRUCTION(add) {

}