#ifndef VOLTAGE_CLIPPER_H_
#define VOLTAGE_CLIPPER_H_

#include "raymath.h"

namespace voltage {

struct Viewport {
  float left, right, top, bottom;
};

int clipTest(float p, float q, float& u1, float& u2) {
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

// Clip a line with Liang–Barsky algorithm
// Implementation idea from: https://www.geeksforgeeks.org/liang-barsky-algorithm/
bool clipLine(Vector2& a, Vector2& b, Viewport& viewport) {
  float u1 = 0;
  float u2 = 1.0;
  float dx = b.x - a.x;
  float dy;

  if (clipTest(-dx, a.x - viewport.left, u1, u2)) {
    if (clipTest(dx, viewport.right - a.x, u1, u2)) {
      dy = b.y - a.y;
      if (clipTest(-dy, a.y - viewport.bottom, u1, u2)) {
        if (clipTest(dy, viewport.top - a.y, u1, u2)) {
          if (u2 < 1.0) {
            b.x = a.x + u2 * dx;
            b.y = a.y + u2 * dy;
          }
          if (u1 > 0.0) {
            a.x += u1 * dx;
            a.y += u1 * dy;
          }
        }
      }
    }
  }
  return false;
}

}  // namespace voltage

#endif
