#ifndef VOLTAGE_MESH_
#define VOLTAGE_MESH_

#include <Arduino.h>

#include "raymath.h"

namespace voltage {

struct Edge {
  uint32_t aIndex, bIndex;
};

class Mesh {
 public:
  uint32_t vertexCount;
  uint32_t edgeCount;
  Vector3* vertices;
  Edge* edges;

  Mesh(Vector3* vertices, uint32_t vertexCount, Edge* edges, uint32_t edgeCount);
  ~Mesh();
};

namespace MeshBuilder {
Mesh* createCube(float size);
Mesh* createPlane(float size);
};  // namespace MeshBuilder

};  // namespace voltage

#endif
