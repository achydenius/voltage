#ifndef VOLTAGE_UTILS_H_
#define VOLTAGE_UTILS_H_

#include "raymath.h"

// Supplementary Vector4 functions for raymath

namespace voltage {

inline Vector4 Vector4Transform(Vector4 v, Matrix matrix) {
  Vector4 result;

  result.x = matrix.m0 * v.x + matrix.m4 * v.y + matrix.m8 * v.z + matrix.m12 * v.w;
  result.y = matrix.m1 * v.x + matrix.m5 * v.y + matrix.m9 * v.z + matrix.m13 * v.w;
  result.z = matrix.m2 * v.x + matrix.m6 * v.y + matrix.m10 * v.z + matrix.m14 * v.w;
  result.w = matrix.m3 * v.x + matrix.m7 * v.y + matrix.m11 * v.z + matrix.m15 * v.w;

  return result;
}

inline Vector4 Vector4Lerp(Vector4 a, Vector4 b, float amount) {
  Vector4 result;

  result.x = a.x + amount * (b.x - a.x);
  result.y = a.y + amount * (b.y - a.y);
  result.z = a.z + amount * (b.z - a.z);
  result.w = a.w + amount * (b.w - a.w);

  return result;
}

}  // namespace voltage

#endif
