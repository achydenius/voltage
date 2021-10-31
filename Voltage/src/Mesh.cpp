#include "Mesh.h"

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
}  // namespace MeshBuilder

}  // namespace voltage
