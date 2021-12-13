#include "Mesh.h"

#include "Array.h"
#include "Utils.h"

namespace voltage {

Mesh::Mesh(const Vector3* sourceVertices, const uint32_t sourceVertexCount, const Edge* sourceEdges,
           const uint32_t sourceEdgeCount) {
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

void Mesh::scale(const float value) {
  for (uint32_t i = 0; i < vertexCount; i++) {
    vertices[i] = Vector3Scale(vertices[i], value);
  }
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

struct EdgeMidpoint {
  uint32_t aIndex, bIndex;
  uint32_t index;
};

uint32_t getMidpoint(const uint32_t aIndex, const uint32_t bIndex, Buffer<Vector3>& vertexBuffer,
                     Buffer<EdgeMidpoint>& midpointBuffer) {
  // TODO: Refactor this to a function and also use it in addEdge?
  uint32_t* midpointIndex = 0;
  for (uint32_t i = 0; i < midpointBuffer.getSize(); i++) {
    if ((aIndex == midpointBuffer[i].aIndex && bIndex == midpointBuffer[i].bIndex) ||
        (aIndex == midpointBuffer[i].bIndex && bIndex == midpointBuffer[i].aIndex)) {
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
Mesh* createIcosphere(float size) {
  const uint32_t icosahedronVertexCount = 12;
  const uint32_t icosahedronTriangleCount = 20;

  const uint32_t iterations = 1;
  const uint32_t vertexCount = 10 * pow(4, iterations) + 2;
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
  Buffer<Triangle> triangleBuffer(triangleCount);
  Buffer<EdgeMidpoint> midpointBuffer(edgeCount);

  // TODO: Replace this with a parameterized constructor?
  for (uint32_t i = 0; i < icosahedronVertexCount; i++) {
    vertexBuffer.push(Vector3Normalize(icosahedronVertices[i]));
  }

  for (uint32_t i = 0; i < icosahedronTriangleCount; i++) {
    Triangle& triangle = icosahedronTriangles[i];

    uint32_t aIndex = getMidpoint(triangle.aIndex, triangle.bIndex, vertexBuffer, midpointBuffer);
    uint32_t bIndex = getMidpoint(triangle.bIndex, triangle.cIndex, vertexBuffer, midpointBuffer);
    uint32_t cIndex = getMidpoint(triangle.cIndex, triangle.aIndex, vertexBuffer, midpointBuffer);

    triangleBuffer.push((Triangle){triangle.aIndex, aIndex, cIndex});
    triangleBuffer.push((Triangle){triangle.bIndex, bIndex, aIndex});
    triangleBuffer.push((Triangle){triangle.cIndex, cIndex, bIndex});
    triangleBuffer.push((Triangle){aIndex, bIndex, cIndex});
  }

  Buffer<Edge> edgeBuffer(edgeCount);
  for (uint32_t i = 0; i < triangleBuffer.getSize(); i++) {
    addEdge((Edge){triangleBuffer[i].aIndex, triangleBuffer[i].bIndex}, edgeBuffer);
    addEdge((Edge){triangleBuffer[i].bIndex, triangleBuffer[i].cIndex}, edgeBuffer);
    addEdge((Edge){triangleBuffer[i].cIndex, triangleBuffer[i].aIndex}, edgeBuffer);
  }

  // TODO: Add conversion method to Buffer class
  // or add a constructor to Mesh that takes a buffer/array as a parameter?
  Edge edges[edgeBuffer.getSize()];
  for (uint32_t i = 0; i < edgeBuffer.getSize(); i++) {
    edges[i] = (Edge){edgeBuffer[i].aIndex, edgeBuffer[i].bIndex};
  }

  Vector3 vertices[vertexBuffer.getSize()];
  for (uint32_t i = 0; i < vertexBuffer.getSize(); i++) {
    vertices[i] = vertexBuffer[i];
  }

  Mesh* mesh = new Mesh(vertices, vertexBuffer.getSize(), edges, edgeBuffer.getSize());
  mesh->scale(size);

  return mesh;
}

}  // namespace MeshBuilder

}  // namespace voltage
