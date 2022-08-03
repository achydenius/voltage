#include <SDL2/SDL.h>

#include "SDL2Writer.h"
#include "emulator.h"

#define VOLTAGE_EMULATOR
#include "../Voltage/src/Voltage.h"

float phase = 0;

voltage::DualDACWriter* writer = new SDL2Writer();
voltage::Renderer renderer(10, *writer);

void tick() {
  renderer.clear();
  renderer.add({{cosf(phase), sinf(phase)}, {cosf(PI + phase), sinf(PI + phase)}});
  renderer.render();

  phase -= 0.001;
}

int main(int argc, char** argv) {
  Emulator emulator;

  emulator.run(tick);

  return 0;
}
