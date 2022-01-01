#ifndef VOLTAGE_ENGINE_H_
#define VOLTAGE_ENGINE_H_

#include "Array.h"
#include "Camera.h"
#include "Clipper.h"
#include "Object.h"
#include "Rasterizer.h"
#include "Renderer.h"
#include "types.h"

namespace voltage {

class Engine {
  static const uint32_t defaultMaxLines = 1000;
  static const uint32_t defaultMaxPoints = 1000;
  Renderer* renderer;
  const Rasterizer rasterizer;
  Buffer<Line2D> lines;
  Buffer<Vector2> points;

  Viewport viewport = {-1.0, 1.0, 0.75, -0.75};
  Vector2 blankingPoint = {1.0, 1.0};

 public:
  Engine(const uint8_t resolutionBits, const Renderer* renderer = nullptr,
         const uint32_t maxLines = defaultMaxLines, const uint32_t maxPoints = defaultMaxPoints)
      : rasterizer(resolutionBits), lines(maxLines), points(maxPoints) {
    if (renderer == nullptr) {
      this->renderer = new LineRenderer(this, maxLines);
    }
  }
  ~Engine() { delete renderer; };

  void setRenderer(Renderer* renderer);
  void setViewport(const Viewport& viewport);
  void setBlankingPoint(const Vector2& blankingPoint);
  void clear();
  void add(const Line2D& line);
  void add(const Vector2& point);
  void add(Object* object, Camera& camera);
  void add(const Array<Object*>& objects, Camera& camera);
  void addViewport();
  void render();
};

}  // namespace voltage

#endif
