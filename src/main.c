#include <gui.h>

int main() {
  gui_t gui;
  init_gui(&gui, "ボケット");

  main_loop(&gui);

  destroy_gui(&gui);
	return 0;
}