#pragma once
#include "register.h"

thumb_handler thumb_handle_data_processing(u16 instruction);

THUMB_INSTRUCTION(unimplemented_data_processing);
THUMB_INSTRUCTION(ldr);