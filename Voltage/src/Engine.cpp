#include "Engine.h"

using namespace voltage;

void Engine::setViewport(const Viewport viewport) { this->viewport = viewport; }

void Engine::setBlankingPoint(const Vector2 blankingPoint) { this->blankingPoint = blankingPoint; }

void Engine::clear() {
  lines.reset();
  points.reset();
}

void Engine::addLine(const Line& line) {
  Vector2 a = line.a;
  Vector2 b = line.b;
  if (clipLine(a, b, viewport)) {
    Line clipped = {a, b};
    lines.add(clipped);
  }
}

void Engine::addPoint(const Vector2& point) {
  if (point.x >= viewport.left && point.x < viewport.right && point.y >= viewport.bottom &&
      point.y < viewport.top) {
    points.add(point);
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
