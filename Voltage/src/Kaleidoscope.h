#include <algorithm>

#include "utils.h"

namespace voltage {

class ClipPolygon {
 public:
  uint32_t vertexCount;
  Vector2* vertices;
  Vector2* normals;

  ClipPolygon(const uint32_t vertexCount) : vertexCount(vertexCount) {
    vertices = new Vector2[vertexCount];
    normals = new Vector2[vertexCount];
  }
  ClipPolygon(const std::initializer_list<Vector2> il) : ClipPolygon(il.size()) {
    std::copy(il.begin(), il.end(), vertices);
    generateNormals();
  }
  ~ClipPolygon() {
    delete vertices;
    delete normals;
  }

 private:
  void generateNormals() {
    for (uint32_t i = 0; i < vertexCount; i++) {
      Vector2 dir = Vector2Subtract(vertices[(i + 1) % vertexCount], vertices[i]);
      normals[i].x = -dir.y;
      normals[i].y = dir.x;
      normals[i] = Vector2Normalize(normals[i]);
    }
  }
};

inline float maxValue(float a, float b) { return a > b ? a : b; }

inline float minValue(float a, float b) { return a < b ? a : b; }

// Clip line against a convex polygon with Cyrus-Beck algorithm:
// https://en.wikipedia.org/wiki/Cyrus%E2%80%93Beck_algorithm
bool clipLine(Vector2& a, Vector2& b, const ClipPolygon& cp) {
  float tE = 0, tL = 1;
  Vector2 P1_P0, PEi;
  P1_P0 = Vector2Subtract(b, a);

  for (uint32_t i = 0; i < cp.vertexCount; i++) {
    PEi = Vector2Subtract(a, cp.vertices[i]);
    float num = Vector2DotProduct(cp.normals[i], PEi);
    float den = Vector2DotProduct(cp.normals[i], P1_P0);

    if (den != 0) {
      float t = num / -den;

      if (den < 0) {
        tE = maxValue(tE, t);  // Entering
      } else {
        tL = minValue(tL, t);  // Exiting
      }
    } else if (num > 0) {  // Parallel and outside
      return false;
    }
  }

  if (tE > tL) {  // Outside
    return false;
  }

  float x0 = a.x;
  float y0 = a.y;

  a.x = x0 + P1_P0.x * tE;
  a.y = y0 + P1_P0.y * tE;
  b.x = x0 + P1_P0.x * tL;
  b.y = y0 + P1_P0.y * tL;
  return true;
}

}  // namespace voltage
