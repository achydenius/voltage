#include "Rasterizer.h"

using namespace voltage;

void Rasterizer::drawPoint(const Vector2 &point, const uint32_t micros) const {
  dacWrite(transform(point.x), transform(point.y));
  delayMicroseconds(100);
}

// Draw a line with Bresenham's line algorithm:
// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
void Rasterizer::drawLine(const Vector2 &a, const Vector2 &b) const {
  int32_t x0 = transform(a.x);
  int32_t y0 = transform(a.y);
  int32_t x1 = transform(b.x);
  int32_t y1 = transform(b.y);

  if (abs(y1 - y0) < abs(x1 - x0)) {
    if (x0 > x1) {
      drawLineLow(x1, y1, x0, y0);
    } else {
      drawLineLow(x0, y0, x1, y1);
    }
  } else {
    if (y0 > y1) {
      drawLineHigh(x1, y1, x0, y0);
    } else {
      drawLineHigh(x0, y0, x1, y1);
    }
  }
}

void Rasterizer::drawLineLow(int32_t x0, int32_t y0, int32_t x1, int32_t y1) const {
  int32_t dx = x1 - x0;
  int32_t dy = y1 - y0;
  int32_t yi = 1;

  if (dy < 0) {
    yi = -1;
    dy = -dy;
  }
  int32_t D = (2 * dy) - dx;
  int32_t y = y0;

  for (int32_t x = x0; x < x1; x++) {
    dacWrite(x, y);
    if (D > 0) {
      y = y + yi;
      D = D + (2 * (dy - dx));
    } else {
      D = D + 2 * dy;
    }
  }
}

void Rasterizer::drawLineHigh(int32_t x0, int32_t y0, int32_t x1, int32_t y1) const {
  int32_t dx = x1 - x0;
  int32_t dy = y1 - y0;
  int32_t xi = 1;

  if (dx < 0) {
    xi = -1;
    dx = -dx;
  }
  int32_t D = (2 * dx) - dy;
  int32_t x = x0;

  for (int32_t y = y0; y < y1; y++) {
    dacWrite(x, y);
    if (D > 0) {
      x = x + xi;
      D = D + (2 * (dx - dy));
    } else {
      D = D + 2 * dx;
    }
  }
}

uint32_t Rasterizer::transform(float value) const {
  return (uint32_t)(value * scaleValueHalf + scaleValueHalf);
}

// Low-level analog write implementation for Teensy 3.6 is copy-pasted here from the core library:
// https://github.com/PaulStoffregen/cores/blob/1.54/teensy3/analog.c#L520-L564
// This improves performance dramatically as the function calls in rendering loop are bypassed.
typedef int16_t __attribute__((__may_alias__)) aliased_int16_t;
void Rasterizer::dacWrite(uint32_t x, uint32_t y) const {
  SIM_SCGC2 |= SIM_SCGC2_DAC0;
  DAC0_C0 = DAC_C0_DACEN | DAC_C0_DACRFS;  // 3.3V VDDA is DACREF_2
  *(volatile aliased_int16_t *)&(DAC0_DAT0L) = x << scaleBits;

  SIM_SCGC2 |= SIM_SCGC2_DAC1;
  DAC1_C0 = DAC_C0_DACEN | DAC_C0_DACRFS;  // 3.3V VDDA is DACREF_2
  *(volatile aliased_int16_t *)&(DAC1_DAT0L) = y << scaleBits;
}
