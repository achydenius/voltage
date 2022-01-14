#ifndef VOLTAGE_RENDERER_H_
#define VOLTAGE_RENDERER_H_

#include "Array.h"
#include "Camera.h"
#include "Object.h"
#include "types.h"

namespace voltage {

struct Vertex {
  Vector4 vector;
  bool isVisible;
};

class Engine;

class Renderer {
 public:
  virtual ~Renderer(){};
  virtual void add(const Array<Object*>& objects, Camera& camera) = 0;
};

class LineRenderer : public Renderer {
  Engine* engine;
  Array<Vertex> processedVertices;
  Buffer<Vertex> clippedVertices;
  Buffer<Line<Vertex*> > processedLines;

 public:
  LineRenderer(Engine* engine, uint32_t maxLines)
      : engine(engine),
        processedVertices(static_cast<size_t>(maxLines * 2)),
        clippedVertices(maxLines),
        processedLines(maxLines) {}

  void add(const Array<Object*>& objects, Camera& camera);

 private:
  void add(Object* object, const Matrix& viewMatrix, const Matrix& projectionMatrix);
};

}  // namespace voltage

#endif
