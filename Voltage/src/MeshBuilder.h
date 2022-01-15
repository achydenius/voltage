#ifndef VOLTAGE_MESH_BUILDER_H_
#define VOLTAGE_MESH_BUILDER_H_

#include "mesh.h"
#include "utils.h"

namespace voltage {
namespace MeshBuilder {

Mesh* createPlane(float size) {
  float half = size / 2;
  Vector3 vertices[] = {{-half, 0, half}, {-half, 0, -half}, {half, 0, -half}, {half, 0, half}};
  FaceDefinition faces[] = {{0, 1, 2, 3}};

  return new Mesh(vertices, 4, faces, 1);
}

Mesh* createCube(float size) {
  float half = size / 2;
  Vector3 vertices[] = {{-half, half, half},  {half, half, half},   {half, half, -half},
                        {-half, half, -half}, {-half, -half, half}, {half, -half, half},
                        {half, -half, -half}, {-half, -half, -half}};
  FaceDefinition faces[] = {{0, 1, 2, 3}, {4, 7, 6, 5}, {0, 4, 5, 1},
                            {1, 5, 6, 2}, {2, 6, 7, 3}, {3, 7, 4, 0}};

  return new Mesh(vertices, 8, faces, 6);
}

struct Triangle {
  uint32_t vertexIndices[3];
};

struct EdgeMidpoint {
  Pair<uint32_t> vertexIndices;
  uint32_t index;
};

bool edgeEquals(const EdgeMidpoint& edge, const Pair<uint32_t>& vertices) {
  return (edge.vertexIndices.a == vertices.a && edge.vertexIndices.b == vertices.b) ||
         (edge.vertexIndices.a == vertices.b && edge.vertexIndices.b == vertices.a);
}

void addEdge(const EdgeMidpoint& edge, Buffer<EdgeMidpoint>& buffer) {
  bool exists = false;
  for (uint32_t i = 0; i < buffer.getSize(); i++) {
    if (edgeEquals(edge, buffer[i].vertexIndices)) {
      exists = true;
    }
  }

  if (!exists) {
    buffer.push(edge);
  }
}

uint32_t getMidpoint(const Pair<uint32_t>& indices, Buffer<Vector3>& vertexBuffer,
                     Buffer<EdgeMidpoint>& midpointBuffer) {
  uint32_t* midpointIndex = 0;
  for (uint32_t i = 0; i < midpointBuffer.getSize(); i++) {
    if (edgeEquals(midpointBuffer[i], indices)) {
      midpointIndex = &midpointBuffer[i].index;
      break;
    }
  }

  if (midpointIndex != 0) {
    return *midpointIndex;
  }

  Vector3& a = vertexBuffer[indices.a];
  Vector3& b = vertexBuffer[indices.b];
  Vector3 midpoint = Vector3Midpoint(a, b);

  uint32_t index = vertexBuffer.getSize();

  vertexBuffer.push(Vector3Normalize(midpoint));
  midpointBuffer.push({indices.a, indices.b, index});

  return index;
}

// Implementation idea from:
// http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html
Mesh* createIcosphere(const float size, const uint32_t subdivisions) {
  const uint32_t icosahedronVertexCount = 12;
  const uint32_t icosahedronTriangleCount = 20;

  const uint32_t vertexCount = 10 * pow(4, (subdivisions - 1)) + 2;
  const uint32_t triangleCount = vertexCount * 2 - 4;
  const uint32_t edgeCount = triangleCount * 3 / 2;

  float t = (1.0 + sqrt(5.0)) / 2.0;

  Vector3 icosahedronVertices[] = {
      {-1, t, 0},  {1, t, 0},  {-1, -t, 0}, {1, -t, 0}, {0, -1, t},  {0, 1, t},
      {0, -1, -t}, {0, 1, -t}, {t, 0, -1},  {t, 0, 1},  {-t, 0, -1}, {-t, 0, 1},
  };
  Triangle icosahedronTriangles[] = {
      // 5 faces around point 0
      {0, 11, 5},
      {0, 5, 1},
      {0, 1, 7},
      {0, 7, 10},
      {0, 10, 11},

      // 5 adjacent faces
      {1, 5, 9},
      {5, 11, 4},
      {11, 10, 2},
      {10, 7, 6},
      {7, 1, 8},

      // 5 faces around point 3
      {3, 9, 4},
      {3, 4, 2},
      {3, 2, 6},
      {3, 6, 8},
      {3, 8, 9},

      // 5 adjacent faces
      {4, 9, 5},
      {2, 4, 11},
      {6, 2, 10},
      {8, 6, 7},
      {9, 8, 1},
  };

  Buffer<Vector3> vertexBuffer(vertexCount);
  Buffer<Triangle> sourceTriangles(triangleCount);
  Buffer<Triangle> targetTriangles(triangleCount);
  Buffer<EdgeMidpoint> midpointBuffer(edgeCount);

  for (uint32_t i = 0; i < icosahedronVertexCount; i++) {
    vertexBuffer.push(Vector3Normalize(icosahedronVertices[i]));
  }

  for (uint32_t i = 0; i < icosahedronTriangleCount; i++) {
    sourceTriangles.push(icosahedronTriangles[i]);
  }

  for (uint32_t i = 1; i < subdivisions; i++) {
    targetTriangles.clear();

    for (uint32_t j = 0; j < sourceTriangles.getSize(); j++) {
      uint32_t* indices = sourceTriangles[j].vertexIndices;

      uint32_t aIndex = getMidpoint({indices[0], indices[1]}, vertexBuffer, midpointBuffer);
      uint32_t bIndex = getMidpoint({indices[1], indices[2]}, vertexBuffer, midpointBuffer);
      uint32_t cIndex = getMidpoint({indices[2], indices[0]}, vertexBuffer, midpointBuffer);

      targetTriangles.push({indices[0], aIndex, cIndex});
      targetTriangles.push({indices[1], bIndex, aIndex});
      targetTriangles.push({indices[2], cIndex, bIndex});
      targetTriangles.push({aIndex, bIndex, cIndex});
    }

    sourceTriangles.clear();
    for (uint32_t j = 0; j < targetTriangles.getSize(); j++) {
      sourceTriangles.push(targetTriangles[j]);
    }
  }

  Vector3 vertices[vertexBuffer.getSize()];
  std::copy(vertexBuffer.getElements(), vertexBuffer.getElements() + vertexBuffer.getSize(),
            vertices);

  FaceDefinition faces[sourceTriangles.getSize()];
  for (uint32_t i = 0; i < sourceTriangles.getSize(); i++) {
    uint32_t* indices = sourceTriangles[i].vertexIndices;
    faces[i] = {indices[0], indices[1], indices[2]};
  }

  Mesh* mesh = new Mesh(vertices, vertexBuffer.getSize(), faces, sourceTriangles.getSize());
  mesh->scale(size);

  return mesh;
}

}  // namespace MeshBuilder
}  // namespace voltage

#endif
