#include "Mesh.h"

#include <algorithm>

namespace voltage {

Mesh::Mesh(const Vector3* sourceVertices, const uint32_t sourceVertexCount, const Face* sourceFaces,
           const uint32_t sourceFaceCount) {
  vertexCount = sourceVertexCount;
  faceCount = sourceFaceCount;

  vertices = new Vector3[vertexCount];
  faces = new Face[faceCount];

  std::copy(sourceVertices, sourceVertices + sourceVertexCount, vertices);
  std::copy(sourceFaces, sourceFaces + sourceFaceCount, faces);

  generateEdges();
}

Mesh::~Mesh() {
  delete vertices;
  delete edges;
  delete faces;
}

void Mesh::scale(const float value) {
  for (uint32_t i = 0; i < vertexCount; i++) {
    vertices[i] = Vector3Scale(vertices[i], value);
  }
}

Edge* Mesh::findEdge(const Edge& edge, const Buffer<Edge>& edges) const {
  for (uint32_t i = 0; i < edges.getSize(); i++) {
    if ((edge.aIndex == edges[i].aIndex && edge.bIndex == edges[i].bIndex) ||
        (edge.aIndex == edges[i].bIndex && edge.bIndex == edges[i].aIndex)) {
      return &edges[i];
    }
  }
  return nullptr;
}

void Mesh::generateEdges() {
  // TODO: Use maxLines constant from the Engine class?
  Buffer<Edge> edgeBuffer(1000);

  for (uint32_t i = 0; i < faceCount; i++) {
    Face& face = faces[i];

    for (uint32_t j = 0; j < face.vertexCount; j++) {
      Edge edge = {face.vertexIndices[j], face.vertexIndices[(j + 1) % face.vertexCount]};
      if (findEdge(edge, edgeBuffer) == nullptr) {
        edgeBuffer.push(edge);
      }
      face.edgeIndices[j] = edgeBuffer.getSize() - 1;
    }
  }

  edges = new Edge[edgeBuffer.getSize()];
  edgeCount = edgeBuffer.getSize();
  std::copy(edgeBuffer.getElements(), edgeBuffer.getElements() + edgeBuffer.getSize(), edges);
}

}  // namespace voltage
