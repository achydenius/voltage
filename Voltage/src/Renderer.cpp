#include "Renderer.h"

#include "Clipper.h"
#include "Engine.h"
#include "Timer.h"
#include "utils.h"

using namespace voltage;

TIMER_CREATE(transform);
TIMER_CREATE(nearClip);
TIMER_CREATE(viewportClip);

void LineRenderer::add(const Array<Object*>& objects, Camera& camera) {
  Matrix matrix = camera.getViewProjectionMatrix();

  for (uint32_t i = 0; i < objects.getCapacity(); i++) {
    add(objects[i], matrix);
  }

  TIMER_SAVE(transform);
  TIMER_SAVE(nearClip);
  TIMER_SAVE(viewportClip);

  TIMER_PRINT(transform);
  TIMER_PRINT(nearClip);
  TIMER_PRINT(viewportClip);
}

void LineRenderer::add(Object* object, const Matrix& viewProjectionMatrix) {
  TIMER_START(transform);

  Matrix modelViewProjectionMatrix = MatrixMultiply(object->getModelMatrix(), viewProjectionMatrix);
  Mesh* mesh = object->mesh;

  // Transform to clip space
  for (uint32_t i = 0; i < mesh->vertexCount; i++) {
    Vector3& vertex = mesh->vertices[i];
    processedVertices[i] = (Vertex){
        Vector4Transform((Vector4){vertex.x, vertex.y, vertex.z, 1.0}, modelViewProjectionMatrix),
        false};
  }
  TIMER_STOP(transform);

  // Clip lines against near plane
  // TODO: Do all clipping in clip space?
  clippedVertices.clear();
  processedLines.clear();

  TIMER_START(nearClip);
  for (uint32_t i = 0; i < mesh->edgeCount; i++) {
    Vertex* ap = &processedVertices[mesh->edges[i].aIndex];
    Vertex* bp = &processedVertices[mesh->edges[i].bIndex];
    Vector4 a = ap->vector;
    Vector4 b = bp->vector;

    ClipResult clipResult = clipLineNear(a, b);

    switch (clipResult) {
      case Outside:
        continue;
      case Inside:
        break;
      case AClipped:
        clippedVertices.push((Vertex){a, false});
        ap = &clippedVertices.getLast();
        break;
      case BClipped:
        clippedVertices.push((Vertex){b, false});
        bp = &clippedVertices.getLast();
        break;
    }
    processedLines.push((Line<Vertex*>){ap, bp});
  }
  TIMER_STOP(nearClip);

  // Project vertices
  for (uint32_t i = 0; i < processedLines.getSize(); i++) {
    Vertex* a = processedLines[i].a;
    Vertex* b = processedLines[i].b;

    TIMER_START(transform);
    if (!a->isProjected) {
      a->vector.x /= a->vector.w;
      a->vector.y /= a->vector.w;
      a->isProjected = true;
    }
    if (!b->isProjected) {
      b->vector.x /= b->vector.w;
      b->vector.y /= b->vector.w;
      b->isProjected = true;
    }
    TIMER_STOP(transform);

    TIMER_START(viewportClip);
    engine->add((Line2D){(Vector2){a->vector.x, a->vector.y}, (Vector2){b->vector.x, b->vector.y}});
    TIMER_STOP(viewportClip);
  }
}
