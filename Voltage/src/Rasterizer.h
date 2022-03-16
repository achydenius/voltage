#ifndef VOLTAGE_RASTERIZER_H_
#define VOLTAGE_RASTERIZER_H_

#include <Arduino.h>

#include "DACWriter.h"
#include "raymath.h"

namespace voltage {

class Rasterizer {
  const DualDACWriter& dacWriter;
  static const uint8_t maxResolutionBits = 12;
  const uint8_t resolutionBits;
  const uint8_t scaleBits;
  const uint32_t scaleValueHalf;

 public:
  Rasterizer(const DualDACWriter& dacWriter, uint8_t resolutionBits)
      : dacWriter(dacWriter),
        resolutionBits(resolutionBits),
        scaleBits(Rasterizer::maxResolutionBits - resolutionBits),
        scaleValueHalf(pow(2, resolutionBits - 1) - 1) {}

  void drawPoint(const Vector2& point) const;
  void drawLine(const Vector2& a, const Vector2& b) const;

 private:
  inline uint32_t transform(float value) const;
  void drawLineLow(int32_t x0, int32_t y0, int32_t x1, int32_t y1) const;
  void drawLineHigh(int32_t x0, int32_t y0, int32_t x1, int32_t y1) const;
};

}  // namespace voltage

#endif
