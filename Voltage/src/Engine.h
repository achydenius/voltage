#ifndef VOLTAGE_ENGINE_H_
#define VOLTAGE_ENGINE_H_

#include "Array.h"
#include "Camera.h"
#include "Clipper.h"
#include "Object.h"
#include "Rasterizer.h"
#include "Renderer.h"
#include "types.h"

namespace voltage {

class Engine {
  static const uint32_t defaultMaxLines = 1000;
  static const uint32_t defaultMaxPoints = 1000;
  Renderer renderer;
  const Rasterizer rasterizer;
  const Teensy36Writer lineWriter;
  const SingleDACWriter* brightnessWriter;
  Buffer<Line> lines;
  Buffer<Point> points;
  Buffer<Line> clippedLines;
  Buffer<Point> clippedPoints;

  Viewport viewport = {-1.0, 1.0, 0.75, -0.75};
  Vector2 blankingPoint = {1.0, 1.0};

 public:
  Engine(uint8_t resolutionBits, SingleDACWriter* brightnessWriter = nullptr,
         uint32_t maxLines = defaultMaxLines, uint32_t maxPoints = defaultMaxPoints)
      : renderer(this, maxLines),
        rasterizer(lineWriter, resolutionBits),
        brightnessWriter(brightnessWriter),
        lines(maxLines),
        points(maxPoints),
        clippedLines(maxLines),
        clippedPoints(maxPoints) {}

  void setViewport(const Viewport& viewport);
  void setBlankingPoint(const Vector2& blankingPoint);
  void clear();
  void add(const Line& line);
  void add(const Point& point);
  void add(Object* object, Camera& camera);
  void add(const Array<Object*>& objects, Camera& camera);
  void addViewport();
  void render();

 private:
  inline uint32_t transformBrightness(float value) const;
};

}  // namespace voltage

#endif
