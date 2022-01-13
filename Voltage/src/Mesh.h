#ifndef VOLTAGE_MESH_H_
#define VOLTAGE_MESH_H_

#define VOLTAGE_MESH_MAX_EDGES 1000

#include <Arduino.h>

#include <algorithm>
#include <initializer_list>

#include "Array.h"
#include "raymath.h"
#include "types.h"

namespace voltage {

struct Vertex {
  Vector3 original;
  Vector4 transformed;
  bool isVisible;
};

struct Face {
  Vertex** vertices;
  uint32_t vertexCount;
  Vector3 normal;
  bool isVisible;
};

struct Edge {
  Pair<Vertex*> vertices;
  Pair<Face*> faces;
  Pair<Vertex*> clipped;
  bool isVisible;
};

class FaceDefinition {
 public:
  uint32_t vertexCount;
  uint32_t* vertexIndices;

  FaceDefinition() : vertexCount(0) {}
  FaceDefinition(const uint32_t vertexCount) : vertexCount(vertexCount) {
    vertexIndices = new uint32_t[vertexCount];
  }
  FaceDefinition(const std::initializer_list<uint32_t> il) : FaceDefinition(il.size()) {
    std::copy(il.begin(), il.end(), vertexIndices);
  }
};

class Mesh {
 public:
  uint32_t vertexCount;
  uint32_t edgeCount;
  uint32_t faceCount;
  Vertex* vertices;
  Edge* edges;
  Face* faces;

  Mesh(const Vector3* vertices, const uint32_t vertexCount, const FaceDefinition* faces,
       const uint32_t faceCount);
  ~Mesh();

  void scale(const float value);

 private:
  Edge* findEdge(const Pair<Vertex*>& vertex, const Buffer<Edge>& edges);
  void generateEdges();
  void generateNormals();
};

};  // namespace voltage

#endif
