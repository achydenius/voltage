#include "Kaleidoscope.h"

#include "Array.h"

using namespace voltage;
using namespace voltage::extra;

void ClipPolygon::generateNormals() {
  for (uint32_t i = 0; i < vertexCount; i++) {
    Vector2 dir = Vector2Subtract(vertices[(i + 1) % vertexCount], vertices[i]);
    normals[i].x = -dir.y;
    normals[i].y = dir.x;
    normals[i] = Vector2Normalize(normals[i]);
  }
}

// Clip line against a convex polygon with Cyrus-Beck algorithm:
// https://en.wikipedia.org/wiki/Cyrus%E2%80%93Beck_algorithm
inline bool clipLine(Vector2& a, Vector2& b, const ClipPolygon& cp) {
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

Buffer<Line2D>& Kaleidoscope::process(const Buffer<Line2D>& buffer) {
  ClipPolygon polygon = {{0, 0}, {1.0, 1.0}, {1.0, 0}};

  lines.clear();

  for (uint32_t i = 0; i < buffer.getSize(); i++) {
    Vector2 a = buffer[i].a;
    Vector2 b = buffer[i].b;
    if (clipLine(a, b, polygon)) {
      lines.push({a, b});
    }
  }

  Matrix2 mirror22AndHalfDegrees = {cos(M_PI_4), sin(M_PI_4), sin(M_PI_4), -cos(M_PI_4)};
  Matrix2 mirror45Degrees = {cos(M_PI_2), sin(M_PI_2), sin(M_PI_2), -cos(M_PI_2)};
  Matrix2 mirrorHorizontal = {-1.0, 0, 0, 1.0};
  Matrix2 mirrorVertical = {1.0, 0, 0, -1.0};

  // mirror(lines, mirror22AndHalfDegrees);
  mirror(lines, mirror45Degrees);
  mirror(lines, mirrorHorizontal);
  mirror(lines, mirrorVertical);

  return lines;
}

void Kaleidoscope::mirror(Buffer<Line2D>& lines, Matrix2& matrix) {
  for (int32_t i = lines.getSize() - 1; i >= 0; i--) {
    lines.push({Vector2Transform(lines[i].a, matrix), Vector2Transform(lines[i].b, matrix)});
  }
}
