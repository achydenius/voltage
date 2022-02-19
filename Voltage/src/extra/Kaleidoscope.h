#include <algorithm>

#include "Renderer.h"
#include "utils.h"

namespace voltage {

namespace extra {

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
  void generateNormals();
};

inline float maxValue(float a, float b) { return a > b ? a : b; }

inline float minValue(float a, float b) { return a < b ? a : b; }

struct Matrix2 {
  float m0, m1;
  float m2, m3;
};

inline Vector2 Vector2Transform(const Vector2& v, const Matrix2& matrix) {
  return {matrix.m0 * v.x + matrix.m2 * v.y, matrix.m1 * v.x + matrix.m3 * v.y};
}

class Kaleidoscope {
  Engine& engine;
  Buffer<Line2D> lines;

 public:
  Kaleidoscope(Engine& engine, uint32_t maxLines) : engine(engine), lines(maxLines) {}

  Buffer<Line2D>& process(const Buffer<Line2D>& buffer);

 private:
  void mirror(Buffer<Line2D>& lines, Matrix2& matrix);
};

}  // namespace extra

}  // namespace voltage
