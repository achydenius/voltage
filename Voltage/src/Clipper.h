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
inline bool clipLine2D(Vector2& a, Vector2& b, const Viewport& vp) {
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

enum {
  OutCodeInside = 0,
  OutCodeNear = 0x1,
  OutCodeFar = 0x2,
  OutCodeLeft = 0x4,
  OutCodeRight = 0x8,
  OutCodeTop = 0x10,
  OutCodeBottom = 0x20
};
typedef uint32_t OutCode;

inline OutCode getOutCode(const Vector4& vector) {
  float epsilon = 0.000001;
  OutCode outCode = 0;

  if (vector.x + vector.w < -epsilon) {
    outCode |= OutCodeLeft;
  } else if (vector.x - vector.w > epsilon) {
    outCode |= OutCodeRight;
  }

  if (vector.y + vector.w < -epsilon) {
    outCode |= OutCodeTop;
  } else if (vector.y - vector.w > epsilon) {
    outCode |= OutCodeBottom;
  }

  if (vector.z + vector.w < -epsilon) {
    outCode |= OutCodeNear;
  } else if (vector.z - vector.w > epsilon) {
    outCode |= OutCodeFar;
  }

  return outCode;
}

inline float getDistanceToPlane(const Vector4& vector, const OutCode outCode) {
  if (outCode & OutCodeLeft) {
    return vector.x + vector.w;
  } else if (outCode & OutCodeRight) {
    return -vector.x + vector.w;
  } else if (outCode & OutCodeTop) {
    return vector.y + vector.w;
  } else if (outCode & OutCodeBottom) {
    return -vector.y + vector.w;
  } else if (outCode & OutCodeNear) {
    return vector.z + vector.w;
  } else if (outCode & OutCodeFar) {
    return -vector.z + vector.w;
  }
}

inline Vector4 clipAgainstPlane(const Vector4& a, const Vector4& b, const OutCode outCode) {
  float da = getDistanceToPlane(a, outCode);
  float db = getDistanceToPlane(b, outCode);
  float t = da / (da - db);
  return Vector4Lerp(a, b, t);
}

// Clip line in homogenous clip space
// Clipping against all planes of the view frustum is done similarly to Cohen-Sutherland algorithm:
// https://en.wikipedia.org/wiki/Cohen%E2%80%93Sutherland_algorithm
inline ClipResult clipLine3D(Vector4& a, Vector4& b) {
  OutCode aOutCode = getOutCode(a);
  OutCode bOutCode = getOutCode(b);

  bool aClipped = false;
  bool bClipped = false;
  while (true) {
    // Both outside
    if (aOutCode & bOutCode) {
      return ClipResult::Outside;
    }

    // Both inside
    if (!(aOutCode | bOutCode)) {
      break;
    }

    OutCode outCodeOut = aOutCode ? aOutCode : bOutCode;

    Vector4 clipped;
    if (outCodeOut & OutCodeLeft) {
      clipped = clipAgainstPlane(a, b, OutCodeLeft);
    } else if (outCodeOut & OutCodeRight) {
      clipped = clipAgainstPlane(a, b, OutCodeRight);
    } else if (outCodeOut & OutCodeTop) {
      clipped = clipAgainstPlane(a, b, OutCodeTop);
    } else if (outCodeOut & OutCodeBottom) {
      clipped = clipAgainstPlane(a, b, OutCodeBottom);
    } else if (outCodeOut & OutCodeNear) {
      clipped = clipAgainstPlane(a, b, OutCodeNear);
    } else if (outCodeOut & OutCodeFar) {
      clipped = clipAgainstPlane(a, b, OutCodeFar);
    }

    if (outCodeOut == aOutCode) {
      a = clipped;
      aOutCode = getOutCode(a);
      aClipped = true;
    } else {
      b = clipped;
      bOutCode = getOutCode(b);
      bClipped = true;
    }
  }

  if (aClipped && bClipped) {
    return ClipResult::BothClipped;
  } else if (aClipped) {
    return ClipResult::AClipped;
  } else {
    return ClipResult::BClipped;
  }
}
}  // namespace voltage

#endif
