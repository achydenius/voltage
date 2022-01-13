#ifndef VOLTAGE_MESH_H_
#define VOLTAGE_MESH_H_

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

class Face {
 public:
  uint32_t vertexCount;
  uint32_t* vertexIndices;
  Vector3 normal;
  bool isVisible;

  Face() : vertexCount(0) {}
  Face(const uint32_t vertexCount) : vertexCount(vertexCount) {
    vertexIndices = new uint32_t[vertexCount];
  }
  Face(const std::initializer_list<uint32_t> il) : Face(il.size()) {
    std::copy(il.begin(), il.end(), vertexIndices);
  }
};

struct Edge {
  Pair<uint32_t> vertexIndices;
  Pair<Face*> faces;
  Pair<Vertex*> clipped;
  bool isVisible;
};

class Mesh {
 public:
  uint32_t vertexCount;
  uint32_t edgeCount;
  uint32_t faceCount;
  Vertex* vertices;
  Edge* edges;
  Face* faces;

  Mesh(const Vector3* vertices, const uint32_t vertexCount, const Face* faces,
       const uint32_t faceCount);
  ~Mesh();

  void scale(const float value);

 private:
  Edge* findEdge(const Pair<uint32_t>& indices, const Buffer<Edge>& edges);
  void generateEdges();
  void generateNormals();
};

};  // namespace voltage

#endif
