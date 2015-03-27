#include "main.hpp"
#include <stdio.h>

Engine engine(ROOT_WINDOW_WIDTH, ROOT_WINDOW_HEIGHT);

int main() {
  while (!TCODConsole::isWindowClosed()) {
    engine.update();
    engine.render();
    TCODConsole::flush();
  }
  return 0;
}
