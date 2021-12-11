#include "Mesh.h"

#include "Array.h"

namespace voltage {

Mesh::Mesh(Vector3* sourceVertices, uint32_t sourceVertexCount, Edge* sourceEdges,
           uint32_t sourceEdgeCount) {
  vertexCount = sourceVertexCount;
  edgeCount = sourceEdgeCount;
  vertices = new Vector3[vertexCount];
  edges = new Edge[edgeCount];

  for (uint32_t i = 0; i < vertexCount; i++) {
    vertices[i] = sourceVertices[i];
  }

  for (uint32_t i = 0; i < edgeCount; i++) {
    edges[i] = sourceEdges[i];
  }
}

Mesh::~Mesh() {
  delete vertices;
  delete edges;
}

namespace MeshBuilder {
Mesh* createCube(float size) {
  float half = size / 2;
  Vector3 vertices[] = {{-half, half, half},  {-half, half, -half}, {half, half, -half},
                        {half, half, half},   {-half, -half, half}, {-half, -half, -half},
                        {half, -half, -half}, {half, -half, half}};
  Edge edges[] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6},
                  {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};

  return new Mesh(vertices, 8, edges, 12);
}

Mesh* createPlane(float size) {
  float half = size / 2;
  Vector3 vertices[] = {{-half, 0, half}, {-half, 0, -half}, {half, 0, -half}, {half, 0, half}};
  Edge edges[] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}};

  return new Mesh(vertices, 4, edges, 4);
}

void addEdge(const Edge& edge, Buffer<Edge>& buffer) {
  bool exists = false;
  for (uint32_t i = 0; i < buffer.getSize(); i++) {
    if ((edge.aIndex == buffer[i].aIndex && edge.bIndex == buffer[i].bIndex) ||
        (edge.aIndex == buffer[i].bIndex && edge.bIndex == buffer[i].aIndex)) {
      exists = true;
      break;
    }
  }

  if (!exists) {
    buffer.push(edge);
  }
}

Mesh* createIcosahedron(float size) {
  const uint32_t vertexCount = 12;
  const uint32_t triangleCount = 20;
  const uint32_t edgeCount = 30;

  float t = (1.0 + sqrt(5.0)) / 2.0;

  Vector3 vertices[] = {
      {-1, t, 0},  {1, t, 0},  {-1, -t, 0}, {1, -t, 0}, {0, -1, t},  {0, 1, t},
      {0, -1, -t}, {0, 1, -t}, {t, 0, -1},  {t, 0, 1},  {-t, 0, -1}, {-t, 0, 1},
  };
  Triangle triangles[] = {
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

  Buffer<Edge> edgeBuffer(edgeCount);
  for (uint32_t i = 0; i < triangleCount; i++) {
    addEdge((Edge){triangles[i].aIndex, triangles[i].bIndex}, edgeBuffer);
    addEdge((Edge){triangles[i].bIndex, triangles[i].cIndex}, edgeBuffer);
    addEdge((Edge){triangles[i].cIndex, triangles[i].aIndex}, edgeBuffer);
  }

  Edge* edges = new Edge[edgeBuffer.getSize()];
  for (uint32_t i = 0; i < edgeBuffer.getSize(); i++) {
    edges[i] = (Edge){edgeBuffer[i].aIndex, edgeBuffer[i].bIndex};
  }

  for (uint32_t i = 0; i < vertexCount; i++) {
    vertices[i] = Vector3Scale(vertices[i], size);
  }

  Mesh* mesh = new Mesh(vertices, 12, edges, 30);

  return mesh;
}

}  // namespace MeshBuilder

}  // namespace voltage
