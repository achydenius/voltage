#ifndef VOLTAGE_RENDERER_H_
#define VOLTAGE_RENDERER_H_

#ifndef VOLTAGE_EMULATOR
#include "DACWriter.h"
#endif

#include "Array.h"
#include "Camera.h"
#include "Clipper.h"
#include "Object.h"
#include "Rasterizer.h"
#include "Transform3D.h"
#include "types.h"

namespace voltage {

class BrightnessTransform {
 protected:
  const uint32_t maxValue;

 public:
  BrightnessTransform(const DACWriter* writer) : maxValue(writer->getMaxValue()) {}
  virtual uint32_t transform(float value) const = 0;
};

class LinearBrightnessTransform : public BrightnessTransform {
 public:
  LinearBrightnessTransform(const DACWriter* writer) : BrightnessTransform(writer) {}
  inline uint32_t transform(float value) const { return (uint32_t)(value * maxValue); }
};

class InvertedLinearBrightnessTransform : public BrightnessTransform {
 public:
  InvertedLinearBrightnessTransform(const DACWriter* writer) : BrightnessTransform(writer) {}
  inline uint32_t transform(float value) const { return (uint32_t)((1.0 - value) * maxValue); }
};

class Renderer {
  static const uint32_t defaultMaxLines = 1000;
  static const uint32_t blankingDrawIncrement = 16;
  const float blankingBrightnessIncrement = 0.015;
  const uint32_t increment;
  Transform3D transform3D;
  const Rasterizer rasterizer;
  const SingleDACWriter* brightnessWriter;
  const BrightnessTransform* brightnessTransform;
  Buffer<Line> lines;
  Buffer<Line> clippedLines;
  Vector2 beamPosition = {0, 0};

#ifndef VOLTAGE_EMULATOR
  Teensy36Writer teensyLineWriter;
#endif

  Viewport viewport = {-1.0, 1.0, 0.75, -0.75};
  Vector2 blankingPoint = {1.0, 1.0};

 public:
  Renderer(const uint32_t increment, DualDACWriter& lineWriter,
           SingleDACWriter* brightnessWriter = nullptr,
           BrightnessTransform* brightnessTransform = nullptr, uint32_t maxLines = defaultMaxLines)
      : increment(increment),
        transform3D(this, maxLines),
        rasterizer(lineWriter),
        brightnessWriter(brightnessWriter),
        brightnessTransform(brightnessTransform),
        lines(maxLines),
        clippedLines(maxLines) {}

#ifndef VOLTAGE_EMULATOR
  Renderer(const uint32_t increment = 1, SingleDACWriter* brightnessWriter = nullptr,
           BrightnessTransform* brightnessTransform = nullptr, uint32_t maxLines = defaultMaxLines)
      : Renderer(increment, teensyLineWriter, brightnessWriter, brightnessTransform, maxLines) {}
#endif

  void setViewport(const Viewport& viewport);
  void setBlankingPoint(const Vector2& blankingPoint);
  void clear();
  void add(const Line& line);
  void add(Object* object, Camera& camera);
  void add(const Array<Object*>& objects, Camera& camera);
  void addViewport();
  void render();
};

}  // namespace voltage

#endif
