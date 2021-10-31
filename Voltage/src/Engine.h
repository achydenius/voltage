#ifndef VOLTAGE_ENGINE_H_
#define VOLTAGE_ENGINE_H_

#include "Array.h"
#include "Camera.h"
#include "Clipper.h"
#include "Object.h"
#include "Renderer.h"

namespace voltage {

struct Line {
  Vector2 a, b;
};

class Engine {
  static const uint32_t defaultMaxLines = 1000;
  static const uint32_t defaultMaxPoints = 1000;
  const Renderer renderer;
  Buffer<Line> lines;
  Buffer<Vector2> points;
  Viewport viewport = {-1.0, 1.0, 0.75, -0.75};
  Vector2 blankingPoint = {1.0, 1.0};

 public:
  Engine(uint8_t resolutionBits, uint32_t maxLines = defaultMaxLines,
         uint32_t maxPoints = defaultMaxPoints)
      : renderer(resolutionBits), lines(maxLines), points(maxPoints) {}
  void setViewport(const Viewport& viewport);
  void setBlankingPoint(const Vector2& blankingPoint);
  void clear();
  void addLine(const Line& line);
  void addPoint(const Vector2& point);
  void addViewport();
  void addObjects(const Array<Object*>& objects, Camera& camera);
  void render();
};

}  // namespace voltage

#endif
