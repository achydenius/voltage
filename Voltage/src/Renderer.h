#ifndef VOLTAGE_RENDERER_H_
#define VOLTAGE_RENDERER_H_

#include "Array.h"
#include "Camera.h"
#include "Object.h"
#include "types.h"

namespace voltage {

class Engine;

class Renderer {
  Engine* engine;
  Buffer<Vertex> clippedVertices;

 public:
  Renderer(Engine* engine, uint32_t maxLines) : engine(engine), clippedVertices(maxLines) {}

  void render(const Array<Object3D*>& objects, Camera& camera);

 private:
  void render(Object3D* object, const Matrix& viewMatrix, const Matrix& projectionMatrix);
};

}  // namespace voltage

#endif
