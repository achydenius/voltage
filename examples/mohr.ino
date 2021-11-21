#include <Voltage.h>

voltage::Engine engine(12);
voltage::Mesh *mesh = voltage::MeshBuilder::createCube(1.0);
voltage::Array<voltage::Object *> objects(6);
voltage::FreeCamera camera;

// Create separate viewports for the upper and lower halfs of the screen
voltage::Array<voltage::Viewport> viewports({{-1.0, 1.0, 0.75, 0}, {-1.0, 1.0, 0, -0.75}});

void setup() {
  // Create six cubes and place them in pairs
  for (int i = 0; i < 6; i++) {
    objects[i] = new voltage::Object(mesh);
    objects[i]->setTranslation(((i / 2) - 1) * 2.0, 0, 0);
  }

  camera.setTranslation(0, 0, 6.0);
}

float phase = 0;
void loop() {
  engine.clear();

  for (int i = 0; i < 6; i += 2) {
    // Rotate a cube pair to opposite directions
    objects[i]->setRotation(-phase, phase, 0);
    objects[i + 1]->setRotation(phase, -phase, 0);

    // Add the cubes to different viewports
    engine.addObject(objects[i], camera, viewports[0]);
    engine.addObject(objects[i + 1], camera, viewports[1]);
  }

  engine.render();

  phase += 0.01;
}
