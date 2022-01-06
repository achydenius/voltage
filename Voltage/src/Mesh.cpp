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
  // TODO: Use maxLines constant from the Engine class?
  Buffer<Edge> edgeBuffer(1000);

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

}  // namespace voltage
