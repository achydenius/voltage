#ifndef VOLTAGE_RENDERER_H_
#define VOLTAGE_RENDERER_H_

// Use Teensy 3.6 core library definitions
#ifndef __MK66FX1M0__
#define __MK66FX1M0__
#endif
#include <Arduino.h>

#include "raymath.h"

namespace voltage {

class Renderer {
  static const uint8_t maxResolutionBits = 12;
  const uint8_t resolutionBits;
  const uint8_t scaleBits;
  const uint32_t scaleValueHalf;

 public:
  Renderer(uint8_t resolutionBits)
      : resolutionBits(resolutionBits),
        scaleBits(Renderer::maxResolutionBits - resolutionBits),
        scaleValueHalf(pow(2, resolutionBits - 1) - 1) {}

  void drawPoint(const Vector2& point) const;
  void drawLine(const Vector2& a, const Vector2& b) const;

 private:
  inline uint32_t transform(float value) const;
  inline void dacWrite(uint32_t x, uint32_t y) const;
  void drawLineLow(int32_t x0, int32_t y0, int32_t x1, int32_t y1) const;
  void drawLineHigh(int32_t x0, int32_t y0, int32_t x1, int32_t y1) const;
};

}  // namespace voltage

#endif
