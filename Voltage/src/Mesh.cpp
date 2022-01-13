#include "Mesh.h"

#include <algorithm>

namespace voltage {

Mesh::Mesh(const Vector3* sourceVertices, const uint32_t sourceVertexCount, const Face* sourceFaces,
           const uint32_t sourceFaceCount) {
  vertexCount = sourceVertexCount;
  faceCount = sourceFaceCount;

  vertices = new Vertex[vertexCount];
  faces = new Face[faceCount];

  for (uint32_t i = 0; i < sourceVertexCount; i++) {
    vertices[i].original = sourceVertices[i];
  }
  std::copy(sourceFaces, sourceFaces + sourceFaceCount, faces);

  generateEdges();
  generateNormals();
}

Mesh::~Mesh() {
  delete vertices;
  delete edges;
  delete faces;
}

void Mesh::scale(const float value) {
  for (uint32_t i = 0; i < vertexCount; i++) {
    vertices[i].original = Vector3Scale(vertices[i].original, value);
  }
}

Edge* Mesh::findEdge(const Pair<uint32_t>& indices, const Buffer<Edge>& edges) {
  for (uint32_t i = 0; i < edges.getSize(); i++) {
    Edge* edge = &edges[i];
    if ((indices.a == edge->vertexIndices.a && indices.b == edge->vertexIndices.b) ||
        (indices.a == edge->vertexIndices.b && indices.b == edge->vertexIndices.a)) {
      return edge;
    }
  }
  return nullptr;
}

void Mesh::generateEdges() {
  Buffer<Edge> edgeBuffer(vertexCount * (vertexCount - 1) / 2);

  for (uint32_t i = 0; i < faceCount; i++) {
    Face& face = faces[i];

    for (uint32_t j = 0; j < face.vertexCount; j++) {
      Pair<uint32_t> indices = {face.vertexIndices[j],
                                face.vertexIndices[(j + 1) % face.vertexCount]};
      Edge* existingEdge = findEdge(indices, edgeBuffer);

      if (existingEdge != nullptr) {
        existingEdge->faces.b = &face;
      } else {
        edgeBuffer.push((Edge){indices.a, indices.b, &face, nullptr});
      }
    }
  }

  edges = new Edge[edgeBuffer.getSize()];
  edgeCount = edgeBuffer.getSize();
  std::copy(edgeBuffer.getElements(), edgeBuffer.getElements() + edgeBuffer.getSize(), edges);
}

void Mesh::generateNormals() {
  for (uint32_t i = 0; i < faceCount; i++) {
    Face& face = faces[i];

    Vector3 a = Vector3Subtract(vertices[face.vertexIndices[1]].original,
                                vertices[face.vertexIndices[0]].original);
    Vector3 b = Vector3Subtract(vertices[face.vertexIndices[2]].original,
                                vertices[face.vertexIndices[0]].original);
    Vector3 normal = Vector3CrossProduct(a, b);
    face.normal = Vector3Normalize(normal);
  }
}

}  // namespace voltage
