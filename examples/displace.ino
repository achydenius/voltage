#include <Voltage.h>

voltage::Engine engine(10);

voltage::Mesh *mesh = voltage::MeshBuilder::createIcosphere(1.0, 3);
voltage::Object3D *object = new voltage::Object3D(mesh);

// Create a vertex array for storing the original icosphere coordinates
Vector3 *vertices = new Vector3[mesh->vertexCount];

voltage::FreeCamera camera;

void setup() {
  // Copy the original icosphere coordinates
  for (unsigned int i = 0; i < mesh->vertexCount; i++) {
    vertices[i] = mesh->vertices[i].original;
  }

  camera.setTranslation(0, 0, 5.0);
}

float phase = 0;
void loop() {
  for (unsigned int i = 0; i < mesh->vertexCount; i++) {
    // Define a scaling factor based on sine, phase and the original position of the vertex
    float scale = sin(((phase * 3.0) + vertices[i].x + vertices[i].y) * 3.0) * 0.5 + 1.0;

    // Scale the original coordinate
    mesh->vertices[i].original = Vector3Scale(vertices[i], scale);
  }

  object->setRotation(0, 0, phase);

  engine.clear();
  engine.add(object, camera);
  engine.render();
  phase += 0.01;
}
