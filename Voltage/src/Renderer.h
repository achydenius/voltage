#ifndef VOLTAGE_RENDERER_H_
#define VOLTAGE_RENDERER_H_

#include "Array.h"
#include "Camera.h"
#include "Clipper.h"
#include "Object.h"
#include "ObjectPipeline.h"
#include "Rasterizer.h"
#include "types.h"

namespace voltage {

class Renderer {
  static const uint32_t defaultMaxLines = 1000;
  static const uint32_t defaultMaxPoints = 1000;
  ObjectPipeline pipeline;
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
  Renderer(uint8_t resolutionBits, SingleDACWriter* brightnessWriter = nullptr,
           uint32_t maxLines = defaultMaxLines, uint32_t maxPoints = defaultMaxPoints)
      : pipeline(this, maxLines),
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
  void render();
  void renderCalibrationView();

 private:
  inline uint32_t transformBrightness(float value) const;
};

}  // namespace voltage

#endif
