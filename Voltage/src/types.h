#ifndef VOLTAGE_TYPES_H_
#define VOLTAGE_TYPES_H_

#include "raymath.h"

namespace voltage {

template <typename T>
struct Pair {
  T a, b;
};

template <typename T>
using Line = Pair<T>;

using Line2D = Pair<Vector2>;

}  // namespace voltage

#endif
