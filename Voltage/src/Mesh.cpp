#ifndef VOLTAGE_EMULATOR
#include <Arduino.h>
#endif

#include <algorithm>

#include "Mesh.h"

using namespace voltage;

Mesh::Mesh(const Vector3* vertices, const uint32_t vertexCount, const FaceDefinition* faces,
           const uint32_t faceCount) {
  setupVerticesAndFaces(vertices, vertexCount, faces, faceCount);
  generateEdges();
  addFaceToEdgePointers();
}

Mesh::Mesh(const Vector3* sourceVertices, const uint32_t sourceVertexCount,
           const FaceDefinition* sourceFaces, const uint32_t sourceFaceCount,
           const EdgeDefinition* sourceEdges, const uint32_t sourceEdgeCount) {
  setupVerticesAndFaces(sourceVertices, sourceVertexCount, sourceFaces, sourceFaceCount);

  edgeCount = sourceEdgeCount;
  edges = new Edge[edgeCount];

  for (uint32_t i = 0; i < sourceEdgeCount; i++) {
    const EdgeDefinition& edge = sourceEdges[i];
    edges[i].vertices = {&vertices[edge.vertexIndices.a], &vertices[edge.vertexIndices.b]};
    edges[i].faces.a = edge.faceIndices.a > -1 ? &faces[edge.faceIndices.a] : nullptr;
    edges[i].faces.b = edge.faceIndices.b > -1 ? &faces[edge.faceIndices.b] : nullptr;
  }

  addFaceToEdgePointers();
}

Mesh::~Mesh() {
  delete vertices;
  delete edges;
  delete faces;
}

void Mesh::setupVerticesAndFaces(const Vector3* sourceVertices, const uint32_t sourceVertexCount,
                                 const FaceDefinition* sourceFaces,
                                 const uint32_t sourceFaceCount) {
  vertexCount = sourceVertexCount;
  faceCount = sourceFaceCount;

  vertices = new Vertex[vertexCount];
  faces = new Face[faceCount];

  for (uint32_t i = 0; i < sourceVertexCount; i++) {
    vertices[i].original = sourceVertices[i];
  }
  for (uint32_t i = 0; i < sourceFaceCount; i++) {
    const FaceDefinition& sourceFace = sourceFaces[i];
    Face& face = faces[i];

    face.vertexCount = sourceFace.vertexCount;
    face.vertices = new Vertex*[face.vertexCount];

    for (uint32_t j = 0; j < sourceFace.vertexCount; j++) {
      face.vertices[j] = &vertices[sourceFace.vertexIndices[j]];
    }
  }

  generateNormals();
  calculateBoundingSphere();
}

void Mesh::scale(const float value) {
  for (uint32_t i = 0; i < vertexCount; i++) {
    vertices[i].original = Vector3Scale(vertices[i].original, value);
  }
}

Edge* Mesh::findEdge(const Pair<Vertex*>& vertices, const Buffer<Edge>& edges) {
  for (uint32_t i = 0; i < edges.getSize(); i++) {
    Edge* edge = &edges[i];
    if ((vertices.a == edge->vertices.a && vertices.b == edge->vertices.b) ||
        (vertices.a == edge->vertices.b && vertices.b == edge->vertices.a)) {
      return edge;
    }
  }
  return nullptr;
}

void Mesh::generateEdges() {
  Buffer<Edge> edgeBuffer(VOLTAGE_MESH_MAX_EDGES);
  for (uint32_t i = 0; i < faceCount; i++) {
    Face& face = faces[i];

    for (uint32_t j = 0; j < face.vertexCount; j++) {
      Pair<Vertex*> edgeVertices = {face.vertices[j], face.vertices[(j + 1) % face.vertexCount]};
      Edge* existingEdge = findEdge(edgeVertices, edgeBuffer);

      if (existingEdge != nullptr) {
        existingEdge->faces.b = &face;
      } else {
        edgeBuffer.push({{edgeVertices.a, edgeVertices.b}, {&face, nullptr}});
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

    Vector3 a = Vector3Subtract(face.vertices[1]->original, face.vertices[0]->original);
    Vector3 b = Vector3Subtract(face.vertices[2]->original, face.vertices[0]->original);
    Vector3 normal = Vector3CrossProduct(a, b);
    face.normal = Vector3Normalize(normal);
  }
}

void Mesh::addFaceToEdgePointers() {
  Buffer<Edge*> edgePointers(VOLTAGE_MESH_MAX_EDGES);

  for (uint32_t i = 0; i < faceCount; i++) {
    Face& face = faces[i];
    edgePointers.clear();

    for (uint32_t j = 0; j < edgeCount; j++) {
      Edge& edge = edges[j];

      if (edge.faces.a == &face) {
        edgePointers.push(&edge);
      } else if (edge.faces.b == &face) {
        edgePointers.push(&edge);
      }
    }

    face.edgeCount = edgePointers.getSize();
    if (edgePointers.getSize() > 0) {
      face.edges = new Edge*[edgePointers.getSize()];
      std::copy(edgePointers.getElements(), edgePointers.getElements() + edgePointers.getSize(),
                face.edges);
    }
  }
}

void Mesh::calculateBoundingSphere() {
  Vector3 min = {0, 0, 0};
  Vector3 max = {0, 0, 0};

  // Calculate axis-aligned bounding box
  for (uint32_t i = 0; i < vertexCount; i++) {
    // TODO: Use Vector3Min/Max instead?
    min.x = fminf(min.x, vertices[i].original.x);
    max.x = fmaxf(max.x, vertices[i].original.x);
    min.y = fminf(min.y, vertices[i].original.y);
    max.y = fmaxf(max.y, vertices[i].original.y);
    min.z = fminf(min.z, vertices[i].original.z);
    max.z = fmaxf(max.z, vertices[i].original.z);
  }

  // Calculate center and radius of the sphere
  float r = 0;
  Vector3 center = Vector3Midpoint(min, max);

  for (uint32_t i = 0; i < vertexCount; i++) {
    r = fmaxf(r, Vector3Distance(vertices[i].original, center));
  }

  boundingSphere = {center.x, center.y, center.z, r};
}
