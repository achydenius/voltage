#ifndef VOLTAGE_UTILS_H_
#define VOLTAGE_UTILS_H_

#include "raymath.h"

// Supplementary functions for raymath

namespace voltage {

inline Vector4 Vector4Transform(const Vector4 &v, const Matrix &matrix) {
  return (Vector4){matrix.m0 * v.x + matrix.m4 * v.y + matrix.m8 * v.z + matrix.m12 * v.w,
                   matrix.m1 * v.x + matrix.m5 * v.y + matrix.m9 * v.z + matrix.m13 * v.w,
                   matrix.m2 * v.x + matrix.m6 * v.y + matrix.m10 * v.z + matrix.m14 * v.w,
                   matrix.m3 * v.x + matrix.m7 * v.y + matrix.m11 * v.z + matrix.m15 * v.w};
}

inline Vector4 Vector4Lerp(const Vector4 &a, const Vector4 &b, const float amount) {
  return (Vector4){a.x + amount * (b.x - a.x), a.y + amount * (b.y - a.y),
                   a.z + amount * (b.z - a.z), a.w + amount * (b.w - a.w)};
}

inline Vector3 Vector3Midpoint(const Vector3 &a, const Vector3 &b) {
  return (Vector3){(a.x + b.x) / 2.0, (a.y + b.y) / 2.0, (a.z + b.z) / 2.0};
}

}  // namespace voltage

#endif
