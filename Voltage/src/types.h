#ifndef VOLTAGE_TYPES_H_
#define VOLTAGE_TYPES_H_

#include "raymath.h"

namespace voltage {

template <typename T>
struct Pair {
  T a, b;
};

struct Line {
  Vector2 a, b;
  float brightness;
};

}  // namespace voltage

#endif
