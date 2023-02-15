#ifndef VOLTAGE_EMULATOR
#include <Arduino.h>
#endif

#include "Rasterizer.h"

using namespace voltage;

void Rasterizer::drawPoint(const Vector2 &point) const {
  dacWriter.write(transform(point.x), transform(point.y));
}

// Draw a line with DDA line drawing algorithm (with increment feature added):
// https://www.geeksforgeeks.org/dda-line-generation-algorithm-computer-graphics/
void Rasterizer::drawLine(const Vector2 &a, const Vector2 &b, const uint32_t increment) const {
  int32_t x0 = transform(a.x);
  int32_t y0 = transform(a.y);
  int32_t x1 = transform(b.x);
  int32_t y1 = transform(b.y);

  float dx = x1 - x0;
  float dy = y1 - y0;

  int32_t steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
  float ix = dx / steps * increment;
  float iy = dy / steps * increment;

  float x = x0;
  float y = y0;

  for (int32_t i = 0; i <= steps; i += increment) {
    dacWriter.write((uint32_t)x, (uint32_t)y);
    x += ix;
    y += iy;
  }
}

uint32_t Rasterizer::transform(float value) const {
  return (uint32_t)(value * scaleValueHalf + scaleValueHalf);
}
