#ifndef VOLTAGE_ENGINE_H_
#define VOLTAGE_ENGINE_H_

#include "Clipper.h"
#include "Renderer.h"

namespace voltage {

struct Line {
  Vector2 a, b;
};

template <typename T>
class Buffer {
  T* data;
  size_t size;
  uint32_t index;

 public:
  Buffer(size_t size) : size(size) { data = new T[size]; }
  ~Buffer() { delete data; };

  T& operator[](uint32_t index) { return data[index]; }
  uint32_t count() { return index; }
  void reset() { index = 0; }
  void add(T d) { data[index++] = d; }
};

class Engine {
  Renderer renderer;
  Viewport viewport = {-1.0, 1.0, 0.75, -0.75};
  Vector2 blankingPoint = {1.0, 1.0};
  Buffer<Line> lines;
  Buffer<Vector2> points;

  static const uint32_t defaultMaxLines = 1000;
  static const uint32_t defaultMaxPoints = 1000;

 public:
  Engine(uint8_t resolutionBits, uint32_t maxLines = defaultMaxLines,
         uint32_t maxPoints = defaultMaxPoints)
      : renderer(resolutionBits), lines(maxLines), points(maxPoints) {}
  void setViewport(const Viewport viewport);
  void setBlankingPoint(const Vector2 blankingPoint);
  void clear();
  void addLine(const Line& line);
  void addPoint(const Vector2& point);
  void addViewport();
  void render();
};

}  // namespace voltage

#endif
