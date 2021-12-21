#ifndef VOLTAGE_MESH_
#define VOLTAGE_MESH_

#include <Arduino.h>

#include "raymath.h"

namespace voltage {

struct Edge {
  uint32_t aIndex, bIndex;
};

struct Triangle {
  uint32_t aIndex, bIndex, cIndex;
};

class Mesh {
 public:
  uint32_t vertexCount;
  uint32_t edgeCount;
  Vector3* vertices;
  Edge* edges;

  Mesh(const Vector3* vertices, const uint32_t vertexCount, const Edge* edges,
       const uint32_t edgeCount);
  ~Mesh();

  void scale(const float value);
};

namespace MeshBuilder {
Mesh* createCube(const float size);
Mesh* createPlane(const float size);
Mesh* createIcosphere(const float size, const uint32_t iterations = 1);
};  // namespace MeshBuilder

};  // namespace voltage

#endif
