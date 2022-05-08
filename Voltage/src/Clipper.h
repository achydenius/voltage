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
enum class ClipResult { Inside, Outside, AClipped, BClipped, BothClipped };

enum class ClipPlane { Near, Far };
enum { CodeNear = 0x1, CodeFar = 0x2 };
typedef int unsigned OutCode;

inline OutCode getOutCode(const Vector4& vector) {
  OutCode outCode = 0;
  if (vector.z < -vector.w) {
    outCode |= CodeNear;
  }
  if (-vector.z < -vector.w) {
    outCode |= CodeFar;
  }
  return outCode;
}

inline float getDistanceToPlane(const Vector4& vector, const ClipPlane plane) {
  return plane == ClipPlane::Near ? vector.z + vector.w : -vector.z + vector.w;
}

inline Vector4 clipAgainstPlane(const Vector4& a, const Vector4& b, const ClipPlane plane) {
  float da = getDistanceToPlane(a, plane);
  float db = getDistanceToPlane(b, plane);
  float t = da / (da - db);
  return Vector4Lerp(a, b, t);
}

inline ClipResult clipLineNearAndFar(Vector4& a, Vector4& b) {
  OutCode aOutCode = getOutCode(a);
  OutCode bOutCode = getOutCode(b);

  // Both outside
  if (aOutCode & bOutCode) {
    return ClipResult::Outside;
  }

  // Both inside
  if (!(aOutCode | bOutCode)) {
    return ClipResult::Inside;
  }

  // A is clipped
  if (aOutCode & CodeNear) {
    a = clipAgainstPlane(a, b, ClipPlane::Near);
  }
  if (aOutCode & CodeFar) {
    a = clipAgainstPlane(a, b, ClipPlane::Far);
  }

  // B is clipped
  if (bOutCode & CodeNear) {
    b = clipAgainstPlane(a, b, ClipPlane::Near);
  }
  if (bOutCode & CodeFar) {
    b = clipAgainstPlane(a, b, ClipPlane::Far);
  }

  if (aOutCode && bOutCode) {
    return ClipResult::BothClipped;
  } else if (aOutCode) {
    return ClipResult::AClipped;
  } else {
    return ClipResult::BClipped;
  }
}
}  // namespace voltage

#endif
