#include "Renderer.h"

#include "Timer.h"

using namespace voltage;

void Renderer::setViewport(const Viewport& viewport) { this->viewport = viewport; }

void Renderer::setBlankingPoint(const Vector2& blankingPoint) {
  this->blankingPoint = blankingPoint;
}

void Renderer::clear() {
  lines.clear();
  points.clear();
}

void Renderer::add(const Line& line) { lines.push(line); }

void Renderer::add(const Point& point) { points.push(point); }

void Renderer::add(Object* object, Camera& camera) {
  static Array<Object*> objects(1);
  objects[0] = object;
  add(objects, camera);
}

void Renderer::add(const Array<Object*>& objects, Camera& camera) {
  pipeline.process(objects, camera);
}

void Renderer::renderCalibrationView() {
  lines.clear();

  Vector2 axesOffset = {0, -0.5};
  Vector2 axes[] = {{0, 0.15}, {0.05, 0.15}, {0, 0.2}, {0, 0}, {0.2, 0}, {0.15, 0.05}, {0.15, 0}};
  for (uint32_t i = 0; i < 6; i++) {
    lines.push({Vector2Add(axes[i], axesOffset), Vector2Add(axes[i + 1], axesOffset), 1.0});
  }

  Vector2 square[] = {{-0.2, 0.2}, {0.2, 0.2}, {0.2, -0.2}, {-0.2, -0.2}};
  for (uint32_t i = 0; i < 4; i++) {
    lines.push({square[i], square[(i + 1) % 4], 1.0});
  }

  Line center[] = {
      {{-0.2, 0.2}, {0.2, -0.2}},
      {{-0.2, -0.2}, {0.2, 0.2}},
  };
  for (uint32_t i = 0; i < 2; i++) {
    lines.push({center[i].a, center[i].b, 1.0});
  }

  Vector2 corners[4][3] = {{{viewport.left, viewport.top - 0.2},
                            {viewport.left, viewport.top},
                            {viewport.left + 0.2, viewport.top}},
                           {{viewport.right - 0.2, viewport.top},
                            {viewport.right, viewport.top},
                            {viewport.right, viewport.top - 0.2}},
                           {{viewport.right, viewport.bottom + 0.2},
                            {viewport.right, viewport.bottom},
                            {viewport.right - 0.2, viewport.bottom}},
                           {{viewport.left + 0.2, viewport.bottom},
                            {viewport.left, viewport.bottom},
                            {viewport.left, viewport.bottom + 0.2}}};

  for (uint32_t i = 0; i < 4; i++) {
    for (uint32_t j = 0; j < 2; j++) {
      lines.push({corners[i][j], corners[i][j + 1], 1.0});
    }
  }

  uint32_t brightnessLineCount = 19;
  float brightnessLinesYOffset = 0.3;
  for (uint32_t i = 0; i < brightnessLineCount; i++) {
    float x = 1.0 / (brightnessLineCount - 1) * i - 0.5;
    float brightness = 1.0 / brightnessLineCount * i;
    lines.push({{x, 0.2 + brightnessLinesYOffset}, {x, brightnessLinesYOffset}, brightness});
  }

  for (uint32_t i = 0; i < lines.getSize(); i++) {
    if (brightnessWriter != nullptr) {
      brightnessWriter->write(transformBrightness(lines[i].brightness));
    }
    rasterizer.drawLine(lines[i].a, lines[i].b);
  }
}

TIMER_CREATE(viewportClip);
TIMER_CREATE(rasterize);

void Renderer::render() {
  TIMER_START(viewportClip);
  clippedLines.clear();
  for (uint32_t i = 0; i < lines.getSize(); i++) {
    Line line = lines[i];
    Vector2 a = line.a;
    Vector2 b = line.b;
    if (clipLine(a, b, viewport)) {
      clippedLines.push({a, b, lines[i].brightness});
    }
  }

  clippedPoints.clear();
  for (uint32_t i = 0; i < points.getSize(); i++) {
    Point point = points[i];
    if (point.x >= viewport.left && point.x < viewport.right && point.y >= viewport.bottom &&
        point.y < viewport.top) {
      clippedPoints.push(point);
    }
  }
  TIMER_STOP(viewportClip);

  TIMER_START(rasterize);
  for (uint32_t i = 0; i < clippedLines.getSize(); i++) {
    if (brightnessWriter != nullptr) {
      brightnessWriter->write(transformBrightness(clippedLines[i].brightness));
    }

    rasterizer.drawLine(clippedLines[i].a, clippedLines[i].b);
  }

  for (uint32_t i = 0; i < clippedPoints.getSize(); i++) {
    if (brightnessWriter != nullptr) {
      brightnessWriter->write(transformBrightness(clippedPoints[i].brightness));
    }
    rasterizer.drawPoint({clippedPoints[i].x, clippedPoints[i].y});
  }
  TIMER_STOP(rasterize);

  TIMER_SAVE(viewportClip);
  TIMER_SAVE(rasterize);

  TIMER_PRINT(viewportClip);
  TIMER_PRINT(rasterize);

  if (brightnessWriter != nullptr) {
    brightnessWriter->write(4095);
  } else {
    // Move beam to blanking point (i.e. outside the screen) when finished drawing
    rasterizer.drawPoint(blankingPoint);
  }
}

uint32_t Renderer::transformBrightness(float value) const { return (uint32_t)(value * 4095.0); }
