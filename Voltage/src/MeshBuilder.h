#ifndef VOLTAGE_MESH_BUILDER_H_
#define VOLTAGE_MESH_BUILDER_H_

#include "mesh.h"
#include "utils.h"

namespace voltage {
namespace MeshBuilder {

Mesh* createPlane(float size) {
  float half = size / 2;
  Vector3 vertices[] = {{-half, 0, half}, {-half, 0, -half}, {half, 0, -half}, {half, 0, half}};
  Face faces[] = {{0, 1, 2, 3}};

  return new Mesh(vertices, 4, faces, 1);
}

Mesh* createCube(float size) {
  float half = size / 2;
  Vector3 vertices[] = {{-half, half, half},  {half, half, half},   {half, half, -half},
                        {-half, half, -half}, {-half, -half, half}, {half, -half, half},
                        {half, -half, -half}, {-half, -half, -half}};
  Face faces[] = {{0, 1, 2, 3}, {4, 5, 6, 7}, {0, 4, 5, 1},
                  {1, 5, 6, 2}, {2, 6, 7, 3}, {3, 7, 4, 0}};

  return new Mesh(vertices, 8, faces, 6);
}

bool edgeEquals(const Edge& edge, const uint32_t aIndex, const uint32_t bIndex) {
  return (edge.aIndex == aIndex && edge.bIndex == bIndex) ||
         (edge.aIndex == bIndex && edge.bIndex == aIndex);
}

void addEdge(const Edge& edge, Buffer<Edge>& buffer) {
  bool exists = false;
  for (uint32_t i = 0; i < buffer.getSize(); i++) {
    if (edgeEquals(edge, buffer[i].aIndex, buffer[i].bIndex)) {
      exists = true;
    }
  }

  if (!exists) {
    buffer.push(edge);
  }
}

struct Triangle {
  uint32_t aIndex, bIndex, cIndex;
};

struct EdgeMidpoint : public Edge {
  uint32_t index;

  EdgeMidpoint() { aIndex = bIndex = index = 0; }
  EdgeMidpoint(uint32_t ai, uint32_t bi, uint32_t index) : index(index) {
    aIndex = ai;
    bIndex = bi;
  }
};

uint32_t getMidpoint(const uint32_t aIndex, const uint32_t bIndex, Buffer<Vector3>& vertexBuffer,
                     Buffer<EdgeMidpoint>& midpointBuffer) {
  uint32_t* midpointIndex = 0;
  for (uint32_t i = 0; i < midpointBuffer.getSize(); i++) {
    if (edgeEquals(midpointBuffer[i], aIndex, bIndex)) {
      midpointIndex = &midpointBuffer[i].index;
      break;
    }
  }

  if (midpointIndex != 0) {
    return *midpointIndex;
  }

  Vector3& a = vertexBuffer[aIndex];
  Vector3& b = vertexBuffer[bIndex];
  Vector3 midpoint = Vector3Midpoint(a, b);

  uint32_t index = vertexBuffer.getSize();

  vertexBuffer.push(Vector3Normalize(midpoint));
  midpointBuffer.push((EdgeMidpoint){aIndex, bIndex, index});

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
      Triangle& triangle = sourceTriangles[j];

      uint32_t aIndex = getMidpoint(triangle.aIndex, triangle.bIndex, vertexBuffer, midpointBuffer);
      uint32_t bIndex = getMidpoint(triangle.bIndex, triangle.cIndex, vertexBuffer, midpointBuffer);
      uint32_t cIndex = getMidpoint(triangle.cIndex, triangle.aIndex, vertexBuffer, midpointBuffer);

      targetTriangles.push((Triangle){triangle.aIndex, aIndex, cIndex});
      targetTriangles.push((Triangle){triangle.bIndex, bIndex, aIndex});
      targetTriangles.push((Triangle){triangle.cIndex, cIndex, bIndex});
      targetTriangles.push((Triangle){aIndex, bIndex, cIndex});
    }

    sourceTriangles.clear();
    for (uint32_t j = 0; j < targetTriangles.getSize(); j++) {
      sourceTriangles.push(targetTriangles[j]);
    }
  }

  Buffer<Edge> edgeBuffer(edgeCount);
  for (uint32_t i = 0; i < targetTriangles.getSize(); i++) {
    addEdge((Edge){targetTriangles[i].aIndex, targetTriangles[i].bIndex}, edgeBuffer);
    addEdge((Edge){targetTriangles[i].bIndex, targetTriangles[i].cIndex}, edgeBuffer);
    addEdge((Edge){targetTriangles[i].cIndex, targetTriangles[i].aIndex}, edgeBuffer);
  }

  Edge edges[edgeBuffer.getSize()];
  std::copy(edgeBuffer.getElements(), edgeBuffer.getElements() + edgeBuffer.getSize(), edges);

  Vector3 vertices[vertexBuffer.getSize()];
  std::copy(vertexBuffer.getElements(), vertexBuffer.getElements() + vertexBuffer.getSize(),
            vertices);

  Face faces[sourceTriangles.getSize()];
  for (uint32_t i = 0; i < sourceTriangles.getSize(); i++) {
    Triangle& triangle = sourceTriangles[i];
    faces[i] = (Face){triangle.aIndex, triangle.bIndex, triangle.cIndex};
  }

  Mesh* mesh = new Mesh(vertices, vertexBuffer.getSize(), faces, sourceTriangles.getSize());
  mesh->scale(size);

  return mesh;
}

}  // namespace MeshBuilder
}  // namespace voltage

#endif
