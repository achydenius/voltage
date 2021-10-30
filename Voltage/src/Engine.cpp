#include "Engine.h"

using namespace voltage;

void Engine::setViewport(const Viewport& viewport) { this->viewport = viewport; }

void Engine::setBlankingPoint(const Vector2& blankingPoint) { this->blankingPoint = blankingPoint; }

void Engine::clear() {
  lines.reset();
  points.reset();
}

void Engine::addLine(const Line& line) {
  Vector2 a = line.a;
  Vector2 b = line.b;
  if (clipLine(a, b, viewport)) {
    lines.add((Line){a, b});
  }
}

void Engine::addPoint(const Vector2& point) {
  if (point.x >= viewport.left && point.x < viewport.right && point.y >= viewport.bottom &&
      point.y < viewport.top) {
    points.add(point);
  }
}

void Engine::addViewport() {
  Vector2 points[] = {
      {viewport.left, viewport.top},
      {viewport.right, viewport.top},
      {viewport.right, viewport.bottom},
      {viewport.left, viewport.bottom},
  };

  for (uint32_t i = 0; i < 4; i++) {
    addLine((Line){points[i], points[(i + 1) % 4]});
  }
}

void Engine::render() {
  for (uint32_t i = 0; i < lines.count(); i++) {
    renderer.drawLine(lines[i].a, lines[i].b);
  }

  for (uint32_t i = 0; i < points.count(); i++) {
    renderer.drawPoint(points[i]);
  }

  // Move beam to blanking point (i.e. outside the screen) when finished drawing
  renderer.drawPoint(blankingPoint);
}

void Engine::render(const Object& object, Camera& camera) {
  // Transform
  Matrix scale = MatrixScale(object.scaling.x, object.scaling.y, object.scaling.z);
  Matrix rotate =
      MatrixRotateXYZ((Vector3){object.rotation.x, object.rotation.y, object.rotation.z});
  Matrix translate =
      MatrixTranslate(object.translation.x, object.translation.y, object.translation.z);
  Matrix matrix =
      MatrixMultiply(MatrixMultiply(MatrixMultiply(scale, rotate), translate), camera.getMatrix());

  for (uint32_t i = 0; i < object.mesh->vertexCount; i++) {
    Vector3 transformed = Vector3Transform(object.mesh->vertices[i], matrix);
    object.projected[i].x = transformed.x / transformed.z;
    object.projected[i].y = transformed.y / transformed.z;
  }

  // Clip
  for (uint32_t i = 0; i < object.mesh->edgeCount; i++) {
    addLine((Line){object.projected[object.mesh->edges[i].aIndex],
                   object.projected[object.mesh->edges[i].bIndex]});
  }

  // Render
  for (uint32_t i = 0; i < lines.count(); i++) {
    renderer.drawLine(lines[i].a, lines[i].b);
  }

  // Move beam to blanking point (i.e. outside the screen) when finished drawing
  renderer.drawPoint(blankingPoint);
}
