#ifndef VOLTAGE_EMULATOR
#include <Arduino.h>
#endif

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

void Renderer::addViewport() {
  Vector2 points[] = {
      {viewport.left, viewport.top},
      {viewport.right, viewport.top},
      {viewport.right, viewport.bottom},
      {viewport.left, viewport.bottom},
  };

  for (uint32_t i = 0; i < 4; i++) {
    add({points[i], points[(i + 1) % 4], 1.0});
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
    // Turn off beam and move it to the next position to be drawn
    if (brightnessWriter != nullptr &&
        (beamPosition.x != clippedLines[i].a.x || beamPosition.y != clippedLines[i].a.y)) {
      brightnessWriter->write(transformBrightness(0));
      rasterizer.drawLine(beamPosition, clippedLines[i].a, blankingIncrement);
      brightnessWriter->write(transformBrightness(clippedLines[i].brightness));
    }

    rasterizer.drawLine(clippedLines[i].a, clippedLines[i].b);
    beamPosition = {clippedLines[i].b.x, clippedLines[i].b.y};
  }

  if (brightnessWriter != nullptr) {
    brightnessWriter->write(transformBrightness(0));
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

  // Turn off beam or move it outside the screen
  if (brightnessWriter != nullptr) {
    brightnessWriter->write(transformBrightness(1.0));
  } else {
    rasterizer.drawPoint(blankingPoint);
  }
}

uint32_t Renderer::transformBrightness(float value) const {
  return (uint32_t)((1.0 - value) * 4095.0);
};
