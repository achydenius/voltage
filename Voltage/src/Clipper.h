#ifndef VOLTAGE_CLIPPER_H_
#define VOLTAGE_CLIPPER_H_

#include "raymath.h"
#include "utils.h"

namespace voltage {

struct Viewport {
  float left, right, top, bottom;
};

static int clipTest(float p, float q, float& u1, float& u2) {
  float r;
  bool value = true;

  if (p < 0) {
    r = q / p;
    if (r > u2) {
      value = false;
    } else if (r > u1) {
      u1 = r;
    }
  } else if (p > 0) {
    r = q / p;
    if (r < u1) {
      value = false;
    } else if (r < u2) {
      u2 = r;
    }
  } else if (q < 0) {
    value = false;
  }

  return value;
}

// Clip a line to viewport with Liang–Barsky algorithm
// Implementation idea from: https://www.geeksforgeeks.org/liang-barsky-algorithm/
inline bool clipLine(Vector2& a, Vector2& b, const Viewport& vp) {
  float u1 = 0;
  float u2 = 1.0;
  Vector2 d = Vector2Subtract(b, a);

  if (!clipTest(-d.x, a.x - vp.left, u1, u2) || !clipTest(d.x, vp.right - a.x, u1, u2) ||
      !clipTest(-d.y, a.y - vp.bottom, u1, u2) || !clipTest(d.y, vp.top - a.y, u1, u2)) {
    return false;
  }

  if (u2 < 1.0) {
    b.x = a.x + u2 * d.x;
    b.y = a.y + u2 * d.y;
  }
  if (u1 > 0.0) {
    a.x += u1 * d.x;
    a.y += u1 * d.y;
  }

  return true;
}

// Return values for 3D clipping
enum ClipResult { Inside, Outside, AClipped, BClipped };

// Clip a line to near plane in homogenous clip space
inline ClipResult clipLineNear(Vector4& a, Vector4& b) {
  // Distance to clipping plane
  float da = a.z + a.w;
  float db = b.z + b.w;

  // Both outside
  if (da < 0 && db < 0) {
    return Outside;
  }
  // Both inside
  if (da >= 0 && db >= 0) {
    return Inside;
  }

  // One of the points is outside
  float t = da / (da - db);

  if (da < 0) {
    a = Vector4Lerp(a, b, t);
    return AClipped;
  } else {
    b = Vector4Lerp(a, b, t);
    return BClipped;
  }
}
}  // namespace voltage

#endif
