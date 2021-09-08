#include <window.h>

int main() {
  window_t window;
  core_t core;

  init_core(&core);
  init_window(&core, &window, "ボケット");

  main_loop(&window, &core);

  destroy_core(&core);
  destroy_window(&window);

	return 0;
}