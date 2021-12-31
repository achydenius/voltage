#include "Renderer.h"

#include "Clipper.h"
#include "Engine.h"
#include "Timer.h"
#include "utils.h"

using namespace voltage;

void LineRenderer::add(const Array<Object*>& objects, Camera& camera) {
  TIMER_CREATE(transform);
  TIMER_CREATE(nearClip);

  Matrix view = camera.getMatrix();

  // Process objects
  for (uint32_t i = 0; i < objects.getCapacity(); i++) {
    Object& object = *objects[i];

    TIMER_START(transform);
    Matrix scale = MatrixScale(object.scaling.x, object.scaling.y, object.scaling.z);
    Matrix rotate =
        MatrixRotateXYZ((Vector3){object.rotation.x, object.rotation.y, object.rotation.z});
    Matrix translate =
        MatrixTranslate(object.translation.x, object.translation.y, object.translation.z);
    Matrix matrix = MatrixMultiply(MatrixMultiply(MatrixMultiply(scale, rotate), translate), view);

    // Transform to clip space
    for (uint32_t i = 0; i < object.mesh->vertexCount; i++) {
      Vector3& vertex = object.mesh->vertices[i];
      processedVertices[i] =
          (Vertex){Vector4Transform((Vector4){vertex.x, vertex.y, vertex.z, 1.0}, matrix), false};
    }
    TIMER_STOP(transform);

    // Clip lines against near plane
    clippedVertices.clear();
    processedLines.clear();

    TIMER_START(nearClip);
    for (uint32_t i = 0; i < object.mesh->edgeCount; i++) {
      Vertex* ap = &processedVertices[object.mesh->edges[i].aIndex];
      Vertex* bp = &processedVertices[object.mesh->edges[i].bIndex];
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

    TIMER_START(transform);
    // Project vertices
    for (uint32_t i = 0; i < processedLines.getSize(); i++) {
      Vertex* a = processedLines[i].a;
      Vertex* b = processedLines[i].b;
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
      engine->add(
          (Line2D){(Vector2){a->vector.x, a->vector.y}, (Vector2){b->vector.x, b->vector.y}});
    }
    TIMER_STOP(transform);
  }

  TIMER_SAVE(transform);
  TIMER_SAVE(nearClip);

  TIMER_PRINT(transform);
  TIMER_PRINT(nearClip);
}
