#ifndef VOLTAGE_UTILS_H_
#define VOLTAGE_UTILS_H_

#include "raymath.h"

// Supplementary functions for raymath

namespace voltage {

struct Matrix3 {
  float m0, m3, m6;
  float m1, m4, m7;
  float m2, m5, m8;
};

inline Matrix3 Matrix3Scale(float x, float y) { return {x, 0, 0, 0, y, 0, 0, 0, 1.0}; }

inline Matrix3 Matrix3Rotate(float angle) {
  Matrix3 result = {1.0, 0, 0, 0, 1.0, 0, 0, 0, 1.0};

  float cosres = cosf(-angle);
  float sinres = sinf(-angle);

  result.m0 = cosres;
  result.m1 = -sinres;
  result.m3 = sinres;
  result.m4 = cosres;

  return result;
}

inline Matrix3 Matrix3Translate(float x, float y) { return {1.0, 0, x, 0, 1.0, y, 0, 0, 1.0}; }

inline Matrix3 Matrix3Multiply(Matrix3 left, Matrix3 right) {
  // TODO: Define and return in a single statement?
  Matrix3 result = {0};

  result.m0 = left.m0 * right.m0 + left.m1 * right.m3 + left.m2 * right.m6;
  result.m1 = left.m0 * right.m1 + left.m1 * right.m4 + left.m2 * right.m7;
  result.m2 = left.m0 * right.m2 + left.m1 * right.m6 + left.m2 * right.m8;

  result.m3 = left.m3 * right.m0 + left.m4 * right.m3 + left.m5 * right.m6;
  result.m4 = left.m3 * right.m1 + left.m4 * right.m4 + left.m5 * right.m7;
  result.m5 = left.m3 * right.m2 + left.m4 * right.m6 + left.m5 * right.m8;

  result.m6 = left.m6 * right.m0 + left.m7 * right.m3 + left.m8 * right.m6;
  result.m7 = left.m6 * right.m1 + left.m7 * right.m4 + left.m8 * right.m7;
  result.m8 = left.m6 * right.m2 + left.m7 * right.m6 + left.m8 * right.m8;

  return result;
}

inline Vector3 Vector3Transform(const Vector3 &v, const Matrix3 &matrix) {
  // TODO: z is actually w?
  return {matrix.m0 * v.x + matrix.m3 * v.y + matrix.m6 * v.z,
          matrix.m1 * v.x + matrix.m4 * v.y + matrix.m7 * v.z,
          matrix.m2 * v.x + matrix.m5 * v.y + matrix.m8 * v.z};
}

inline Vector4 Vector4Transform(const Vector4 &v, const Matrix &matrix) {
  return {matrix.m0 * v.x + matrix.m4 * v.y + matrix.m8 * v.z + matrix.m12 * v.w,
          matrix.m1 * v.x + matrix.m5 * v.y + matrix.m9 * v.z + matrix.m13 * v.w,
          matrix.m2 * v.x + matrix.m6 * v.y + matrix.m10 * v.z + matrix.m14 * v.w,
          matrix.m3 * v.x + matrix.m7 * v.y + matrix.m11 * v.z + matrix.m15 * v.w};
}

inline Vector4 Vector4Lerp(const Vector4 &a, const Vector4 &b, const float amount) {
  return {a.x + amount * (b.x - a.x), a.y + amount * (b.y - a.y), a.z + amount * (b.z - a.z),
          a.w + amount * (b.w - a.w)};
}

inline Vector3 Vector3Midpoint(const Vector3 &a, const Vector3 &b) {
  return {(a.x + b.x) / 2.0, (a.y + b.y) / 2.0, (a.z + b.z) / 2.0};
}

}  // namespace voltage

#endif
