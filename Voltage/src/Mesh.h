#ifndef VOLTAGE_MESH_H_
#define VOLTAGE_MESH_H_

#define VOLTAGE_MESH_MAX_EDGES 1000

#include <Arduino.h>

#include <algorithm>
#include <initializer_list>

#include "Array.h"
#include "types.h"
#include "utils.h"

namespace voltage {

class Vertex {
 public:
  Vector3 original;
  Vector4 transformed;
  bool isVisible;

  void perspectiveDivide() {
    float div = 1.0 / transformed.w;
    transformed.x *= div;
    transformed.y *= div;
    transformed.z *= div;
  }
};

struct Edge {
  Pair<Vertex*> vertices;
  Pair<struct Face*> faces;
  Pair<Vertex*> clipped;
  bool isVisible;
  bool isCulled;
};

class Face {
 public:
  Vertex** vertices;
  uint32_t vertexCount;
  Edge** edges;
  uint32_t edgeCount;
  Vector3 normal;
  bool isVisible;

  float getNormalAngle(const Vector3& vector) {
    Vector3 view = Vector3Subtract(vector, vertices[0]->original);
    return Vector3DotProduct(view, normal);
  }

  void setVerticesVisible(bool isVisible) {
    for (uint32_t i = 0; i < vertexCount; i++) {
      vertices[i]->isVisible = isVisible;
    }
  }
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

class EdgeDefinition {
 public:
  Pair<uint32_t> vertexIndices;
  // An edge can belong to one or two faces.
  // The former case can is denoted by setting the b index as -1
  Pair<int32_t> faceIndices;
};

class Mesh {
 public:
  uint32_t vertexCount;
  uint32_t edgeCount;
  uint32_t faceCount;
  Vertex* vertices;
  Edge* edges;
  Face* faces;
  Vector4 boundingSphere;

  Mesh(const Vector3* vertices, const uint32_t vertexCount, const FaceDefinition* faces,
       const uint32_t faceCount);
  Mesh(const Vector3* vertices, const uint32_t vertexCount, const FaceDefinition* faces,
       const uint32_t faceCount, const EdgeDefinition* edges, const uint32_t edgeCount);

  ~Mesh();

  void scale(const float value);
  void transformVisibleVertices(const Matrix& matrix) {
    for (uint32_t i = 0; i < vertexCount; i++) {
      if (vertices[i].isVisible) {
        Vector3& original = vertices[i].original;
        vertices[i].transformed =
            Vector4Transform({original.x, original.y, original.z, 1.0}, matrix);
      }
    }
  }
  void setVerticesVisible(bool isVisible) {
    for (uint32_t i = 0; i < vertexCount; i++) {
      vertices[i].isVisible = isVisible;
    }
  }

 private:
  void setupVerticesAndFaces(const Vector3* vertices, const uint32_t vertexCount,
                             const FaceDefinition* faces, const uint32_t faceCount);
  Edge* findEdge(const Pair<Vertex*>& vertex, const Buffer<Edge>& edges);
  void generateEdges();
  void generateNormals();
  void addFaceToEdgePointers();
  void calculateBoundingSphere();
};

};  // namespace voltage

#endif
