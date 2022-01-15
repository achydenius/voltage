#include "Engine.h"

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
    lines.push({a, b});
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

void Engine::add(const Array<Object*>& objects, Camera& camera) { renderer->add(objects, camera); }

void Engine::addViewport() {
  Vector2 points[] = {
      {viewport.left, viewport.top},
      {viewport.right, viewport.top},
      {viewport.right, viewport.bottom},
      {viewport.left, viewport.bottom},
  };

  for (uint32_t i = 0; i < 4; i++) {
    add({points[i], points[(i + 1) % 4]});
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
