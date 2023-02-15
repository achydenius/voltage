#ifndef VOLTAGE_RENDERER_H_
#define VOLTAGE_RENDERER_H_

#ifndef VOLTAGE_EMULATOR
#include "DACWriter.h"
#endif

#include "Array.h"
#include "Camera.h"
#include "Clipper.h"
#include "Object.h"
#include "ObjectPipeline.h"
#include "Rasterizer.h"
#include "types.h"

namespace voltage {

class BrightnessTransform {
 public:
  virtual uint32_t transform(float value) const = 0;
};

class LinearBrightnessTransform : public BrightnessTransform {
 public:
  inline uint32_t transform(float value) const { return (uint32_t)(value * 4095.0); }
};

class InvertedLinearBrightnessTransform : public BrightnessTransform {
 public:
  inline uint32_t transform(float value) const { return (uint32_t)((1.0 - value) * 4095.0); }
};

class Renderer {
  static const uint32_t defaultMaxLines = 1000;
  static const uint32_t defaultMaxPoints = 1000;
  static const uint32_t blankingDrawIncrement = 16;
  const float blankingBrightnessIncrement = 0.015;
  ObjectPipeline pipeline;
  const Rasterizer rasterizer;
  const SingleDACWriter* brightnessWriter;
  const BrightnessTransform* brightnessTransform;
  Buffer<Line> lines;
  Buffer<Point> points;
  Buffer<Line> clippedLines;
  Buffer<Point> clippedPoints;
  Vector2 beamPosition = {0, 0};

#ifndef VOLTAGE_EMULATOR
  Teensy36Writer teensyLineWriter;
#endif

  Viewport viewport = {-1.0, 1.0, 0.75, -0.75};
  Vector2 blankingPoint = {1.0, 1.0};

 public:
  float lightIntensity = 10;

  Renderer(uint8_t resolutionBits, const DualDACWriter& lineWriter,
           SingleDACWriter* brightnessWriter = nullptr,
           BrightnessTransform* brightnessTransform = nullptr, uint32_t maxLines = defaultMaxLines,
           uint32_t maxPoints = defaultMaxPoints)
      : pipeline(this, maxLines),
        rasterizer(lineWriter, resolutionBits),
        brightnessWriter(brightnessWriter),
        brightnessTransform(brightnessTransform),
        lines(maxLines),
        points(maxPoints),
        clippedLines(maxLines),
        clippedPoints(maxPoints) {}

#ifndef VOLTAGE_EMULATOR
  Renderer(uint8_t resolutionBits, SingleDACWriter* brightnessWriter = nullptr,
           BrightnessTransform* brightnessTransform = nullptr, uint32_t maxLines = defaultMaxLines,
           uint32_t maxPoints = defaultMaxPoints)
      : Renderer(resolutionBits, teensyLineWriter, brightnessWriter, brightnessTransform, maxLines,
                 maxPoints) {}
#endif

  void setViewport(const Viewport& viewport);
  void setBlankingPoint(const Vector2& blankingPoint);
  void clear();
  void add(const Line& line);
  void add(const Point& point);
  void add(Object* object, Camera& camera);
  void add(const Array<Object*>& objects, Camera& camera);
  void addViewport();
  void render();
};

}  // namespace voltage

#endif
