#ifndef VOLTAGE_RENDERER_H_
#define VOLTAGE_RENDERER_H_

// Use Teensy 3.6 core library definitions
#ifndef __MK66FX1M0__
#define __MK66FX1M0__
#endif
#include <Arduino.h>

namespace voltage {

class Renderer {
  static const uint8_t maxResolutionBits = 12;
  const uint8_t resolutionBits;
  const uint8_t scaleBits;

 public:
  Renderer(uint8_t resolutionBits)
      : resolutionBits(resolutionBits), scaleBits(Renderer::maxResolutionBits - resolutionBits) {}

  void drawPoint(uint16_t x, uint16_t y);
  void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

 private:
  inline void dacWrite(uint32_t x, uint32_t y);
  void drawLineLow(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
  void drawLineHigh(int32_t x0, int32_t y0, int32_t x1, int32_t y1);
};

}  // namespace voltage

#endif
