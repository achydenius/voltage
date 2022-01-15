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
  Mesh* mesh = object->mesh;

  // Transform camera to model space and perform face culling
  // If culling is disabled, mark all faces and vertices visible
  TIMER_START(faceCulling);
  Matrix modelViewMatrix = MatrixMultiply(object->getModelMatrix(), viewMatrix);
  Matrix viewModelMatrix = MatrixInvert(modelViewMatrix);
  Vector3 cameraPosition = Vector3Transform({0, 0, 0}, viewModelMatrix);

  mesh->setVerticesVisible(false);

  for (uint32_t i = 0; i < mesh->faceCount; i++) {
    Face& face = mesh->faces[i];
    if (object->faceCulling == None) {
      face.isVisible = true;
    } else {
      float angle = face.getNormalAngle(cameraPosition);

      if (object->faceCulling == Front) {
        face.isVisible = angle < 0;
      } else {
        face.isVisible = angle > 0;
      }
    }

    if (face.isVisible) {
      face.setVerticesVisible(true);
    }
  }
  TIMER_STOP(faceCulling);

  // Transform to clip space
  TIMER_START(transform);
  Matrix modelViewProjectionMatrix = MatrixMultiply(modelViewMatrix, projectionMatrix);
  mesh->transformVertices(modelViewProjectionMatrix);
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
        clippedVertices.push({{0}, a, true});
        ap = &clippedVertices.getLast();
        break;
      case BClipped:
        clippedVertices.push({{0}, b, true});
        bp = &clippedVertices.getLast();
        break;
    }

    edge.clipped = {ap, bp};
    edge.isVisible = true;
  }
  TIMER_STOP(nearClip);

  // Perspective divide visible original and clipper-generated vertices
  TIMER_START(transform);
  for (uint32_t i = 0; i < mesh->vertexCount; i++) {
    Vertex& vertex = mesh->vertices[i];
    if (vertex.isVisible) {
      vertex.perspectiveDivide();
    }
  }
  for (uint32_t i = 0; i < clippedVertices.getSize(); i++) {
    clippedVertices[i].perspectiveDivide();
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
