#include "Renderer.h"

#include "Clipper.h"
#include "Engine.h"
#include "Timer.h"
#include "utils.h"

using namespace voltage;

TIMER_CREATE(transform);
TIMER_CREATE(nearClip);
TIMER_CREATE(viewportClip);
TIMER_CREATE(faceCulling);

void LineRenderer::add(const Array<Object*>& objects, Camera& camera) {
  Matrix viewMatrix = camera.getViewMatrix();
  Matrix projectionMatrix = camera.getProjectionMatrix();

  for (uint32_t i = 0; i < objects.getCapacity(); i++) {
    add(objects[i], viewMatrix, projectionMatrix);
  }

  TIMER_SAVE(transform);
  TIMER_SAVE(nearClip);
  TIMER_SAVE(viewportClip);
  TIMER_SAVE(faceCulling);

  TIMER_PRINT(transform);
  TIMER_PRINT(nearClip);
  TIMER_PRINT(viewportClip);
  TIMER_PRINT(faceCulling);
}

void LineRenderer::add(Object* object, const Matrix& viewMatrix, const Matrix& projectionMatrix) {
  Matrix modelViewMatrix = MatrixMultiply(object->getModelMatrix(), viewMatrix);

  Mesh* mesh = object->mesh;

  // Transform camera to model space and perform face culling
  TIMER_START(faceCulling);

  for (uint32_t i = 0; i < mesh->vertexCount; i++) {
    mesh->vertices[i].isVisible = false;
  }

  Matrix viewModelMatrix = MatrixInvert(modelViewMatrix);
  Vector3 camera = Vector3Transform({0, 0, 0}, viewModelMatrix);

  for (uint32_t i = 0; i < mesh->faceCount; i++) {
    Face& face = mesh->faces[i];
    if (object->faceCulling == None) {
      face.isVisible = true;
    } else {
      Vector3 vertex = face.vertices[0]->original;
      Vector3 view = Vector3Subtract(camera, vertex);
      float angle = Vector3DotProduct(view, face.normal);

      if (object->faceCulling == Front) {
        face.isVisible = angle < 0;
      } else {
        face.isVisible = angle > 0;
      }
    }

    if (face.isVisible) {
      for (uint32_t j = 0; j < face.vertexCount; j++) {
        face.vertices[j]->isVisible = true;
      }
    }
  }
  TIMER_STOP(faceCulling);

  // Transform to clip space
  TIMER_START(transform);
  Matrix modelViewProjectionMatrix = MatrixMultiply(modelViewMatrix, projectionMatrix);

  for (uint32_t i = 0; i < mesh->vertexCount; i++) {
    Vector3& vertex = mesh->vertices[i].original;
    mesh->vertices[i].transformed =
        Vector4Transform({vertex.x, vertex.y, vertex.z, 1.0}, modelViewProjectionMatrix);
  }
  TIMER_STOP(transform);

  // Clip lines against near plane
  // TODO: Do all clipping in clip space?
  clippedVertices.clear();

  TIMER_START(nearClip);
  for (uint32_t i = 0; i < mesh->edgeCount; i++) {
    Edge& edge = mesh->edges[i];
    Vertex* ap = edge.vertices.a;
    Vertex* bp = edge.vertices.b;

    edge.isVisible = false;

    if (!ap->isVisible || !bp->isVisible) {
      continue;
    }

    Vector4 a = ap->transformed;
    Vector4 b = bp->transformed;

    ClipResult clipResult = clipLineNear(a, b);

    switch (clipResult) {
      case Outside:
        ap->isVisible = false;
        bp->isVisible = false;
        continue;
      case Inside:
        break;
      case AClipped:
        clippedVertices.push((Vertex){{0}, a, true});
        ap = &clippedVertices.getLast();
        break;
      case BClipped:
        clippedVertices.push((Vertex){{0}, b, true});
        bp = &clippedVertices.getLast();
        break;
    }

    edge.clipped = {ap, bp};
    edge.isVisible = true;
  }
  TIMER_STOP(nearClip);

  // Project vertices
  TIMER_START(transform);
  for (uint32_t i = 0; i < mesh->vertexCount; i++) {
    Vertex& vertex = mesh->vertices[i];
    if (vertex.isVisible) {
      vertex.transformed.x /= vertex.transformed.w;
      vertex.transformed.y /= vertex.transformed.w;
    }
  }
  for (uint32_t i = 0; i < clippedVertices.getSize(); i++) {
    Vector4& transformed = clippedVertices[i].transformed;
    transformed.x /= transformed.w;
    transformed.y /= transformed.w;
  }
  TIMER_STOP(transform);

  for (uint32_t i = 0; i < mesh->edgeCount; i++) {
    Edge& edge = mesh->edges[i];
    Vector4& a = edge.clipped.a->transformed;
    Vector4& b = edge.clipped.b->transformed;

    TIMER_START(viewportClip);
    if (edge.isVisible) {
      engine->add({{a.x, a.y}, {b.x, b.y}});
    }
    TIMER_STOP(viewportClip);
  }
}
