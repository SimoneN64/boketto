#include <disasm.h>
#include <log.h>

void init_disasm(disasm_t* debugger) {
  if(cs_open(CS_ARCH_ARM, CS_MODE_ARM | CS_MODE_LITTLE_ENDIAN, &debugger->handle) != CS_ERR_OK) {
    logfatal("Could not initialize capstone\n");
  }
}

void destroy_disasm(disasm_t* disasm) {
  cs_close(&disasm->handle);
}