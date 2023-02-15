#ifndef VOLTAGE_RASTERIZER_H_
#define VOLTAGE_RASTERIZER_H_

#include "Writer.h"
#include "raymath.h"

namespace voltage {

class Rasterizer {
  const DualDACWriter& dacWriter;
  const uint32_t scaleValueHalf;

 public:
  Rasterizer(const DualDACWriter& dacWriter)
      : dacWriter(dacWriter), scaleValueHalf((uint32_t)(dacWriter.getMaxValue() * 0.5)) {}

  void drawPoint(const Vector2& point) const;
  void drawLine(const Vector2& a, const Vector2& b, const uint32_t increment = 1) const;

 private:
  inline uint32_t transform(float value) const;
};

}  // namespace voltage

#endif
