#include "window.h"

int main() {
  window_t window;
	core_t core;
	
	init_window(&window, "ボケット", GBA_W * 3, GBA_H * 3);
	init_core(&core);

	main_loop(&window, &core);

	destroy_core(&core);
	destroy_window(&window);

	return 0;
}