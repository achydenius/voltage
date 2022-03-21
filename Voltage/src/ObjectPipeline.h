#ifndef VOLTAGE_OBJECT_PIPELINE_H_
#define VOLTAGE_OBJECT_PIPELINE_H_

#include "Array.h"
#include "Camera.h"
#include "Object.h"
#include "types.h"

namespace voltage {

class Engine;

class ObjectPipeline {
  Engine* engine;
  Buffer<Vertex> clippedVertices;

 public:
  ObjectPipeline(Engine* engine, uint32_t maxLines) : engine(engine), clippedVertices(maxLines) {}

  void render(const Array<Object*>& objects, Camera& camera);

 private:
  void render(Object* object, const Matrix& viewMatrix, const Matrix& projectionMatrix);
};

}  // namespace voltage

#endif
