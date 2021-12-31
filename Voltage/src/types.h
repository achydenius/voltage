#ifndef VOLTAGE_TYPES_H_
#define VOLTAGE_TYPES_H_

#include "raymath.h"

namespace voltage {

template <typename T>
struct Line {
  T a, b;
};

using Line2D = Line<Vector2>;

}  // namespace voltage

#endif
