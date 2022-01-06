#ifndef VOLTAGE_MESH_H_
#define VOLTAGE_MESH_H_

#include <Arduino.h>

#include <algorithm>
#include <initializer_list>

#include "Array.h"
#include "raymath.h"

namespace voltage {

struct Edge {
  uint32_t aIndex, bIndex;
};

class Face {
 public:
  uint32_t vertexCount;
  uint32_t* vertexIndices;
  uint32_t* edgeIndices;

  Face() : vertexCount(0) {}
  Face(const uint32_t vertexCount) : vertexCount(vertexCount) {
    vertexIndices = new uint32_t[vertexCount];
    edgeIndices = new uint32_t[vertexCount];
  }
  Face(const std::initializer_list<uint32_t> il) : Face(il.size()) {
    std::copy(il.begin(), il.end(), vertexIndices);
  }
};

class Mesh {
 public:
  uint32_t vertexCount;
  uint32_t edgeCount;
  uint32_t faceCount;
  Vector3* vertices;
  Edge* edges;
  Face* faces;

  Mesh(const Vector3* vertices, const uint32_t vertexCount, const Face* faces,
       const uint32_t faceCount);
  ~Mesh();

  void scale(const float value);

 private:
  Edge* findEdge(const Edge& edge, const Buffer<Edge>& edges) const;
  void generateEdges();
};

};  // namespace voltage

#endif
