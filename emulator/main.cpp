#include "emulator.h"

using namespace voltage;

// Emulator and writer need to be instantiated before Renderer
// The size of the emulator window is defined in bits, e.g. 10 yields a 1024 x 1024 size window
Emulator emulator(10);
DualDACWriter* writer = emulator.createWriter();

// the rest of the code can be written like in Arduino IDE,
// the writer just needs to be passed to the renderer
Renderer renderer(10, *writer);
Mesh* mesh = MeshBuilder::createCube(1.0);
Object* object = new Object(mesh);
FreeCamera camera;

void setup() {}

float phase = 0;
void loop() {
  camera.setTranslation(0, 0, 5.0);
  object->setRotation(phase, phase, 0);

  renderer.clear();
  renderer.add(object, camera);
  renderer.render();
  phase += 0.01;
}

// The main function can be left as is
int main(int argc, char** argv) {
  setup();
  emulator.run(loop);
  return 0;
}
