#include "Engine.h"

// Uncomment for enabling performance profiling
// #define VOLTAGE_PROFILE_SAMPLES 100
// #define VOLTAGE_PROFILE

#include "Timer.h"

using namespace voltage;

void Engine::setViewport(const Viewport& viewport) { this->viewport = viewport; }

void Engine::setBlankingPoint(const Vector2& blankingPoint) { this->blankingPoint = blankingPoint; }

void Engine::clear() {
  lines.clear();
  points.clear();
}

void Engine::add(const Line2D& line) {
  Vector2 a = line.a;
  Vector2 b = line.b;
  if (clipLine(a, b, viewport)) {
    lines.push((Line2D){a, b});
  }
}

void Engine::add(const Vector2& point) {
  if (point.x >= viewport.left && point.x < viewport.right && point.y >= viewport.bottom &&
      point.y < viewport.top) {
    points.push(point);
  }
}

void Engine::add(Object* object, Camera& camera) {
  static Array<Object*> objects(1);
  objects[0] = object;
  add(objects, camera);
}

void Engine::add(const Array<Object*>& objects, Camera& camera) {
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
      add((Line2D){(Vector2){a->vector.x, a->vector.y}, (Vector2){b->vector.x, b->vector.y}});
    }
    TIMER_STOP(transform);
  }

  TIMER_SAVE(transform);
  TIMER_SAVE(nearClip);

  TIMER_PRINT(transform);
  TIMER_PRINT(nearClip);
}

void Engine::addViewport() {
  Vector2 points[] = {
      {viewport.left, viewport.top},
      {viewport.right, viewport.top},
      {viewport.right, viewport.bottom},
      {viewport.left, viewport.bottom},
  };

  for (uint32_t i = 0; i < 4; i++) {
    add((Line2D){points[i], points[(i + 1) % 4]});
  }
}

void Engine::render() {
  TIMER_CREATE(render);
  TIMER_START(render);
  for (uint32_t i = 0; i < lines.getSize(); i++) {
    rasterizer.drawLine(lines[i].a, lines[i].b);
  }

  for (uint32_t i = 0; i < points.getSize(); i++) {
    rasterizer.drawPoint(points[i]);
  }
  TIMER_STOP(render);
  TIMER_SAVE(render);

  TIMER_PRINT(render);

  // Move beam to blanking point (i.e. outside the screen) when finished drawing
  rasterizer.drawPoint(blankingPoint);
}
