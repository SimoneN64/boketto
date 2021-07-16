#include "file.h"

void open_file(core_t* core) {
	nfdfilteritem_t filter = {"GameBoy Advance roms", "gba"};
	nfdresult_t result = NFD_OpenDialog(&core->rom_file, &filter, 1, "roms/");
	if(result == NFD_OKAY) {
		load_rom(core);
	}
}