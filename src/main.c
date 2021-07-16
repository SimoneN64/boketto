#include "window.h"

int main() {
  window_t window;
	init_window(&window, "ボケット", GBA_W * 3, GBA_H * 3);

	core_t core;
	init_core(&core);

	main_loop(&window, &core);

	destroy_window(&window);
	destroy_core(&core);
	SDL_Quit();

	return 0;
}