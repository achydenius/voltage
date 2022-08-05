#ifndef VOLTAGE_OBJECT_PIPELINE_H_
#define VOLTAGE_OBJECT_PIPELINE_H_

#include "Array.h"
#include "Camera.h"
#include "Object.h"
#include "types.h"

namespace voltage {

class Renderer;

class ObjectPipeline {
  Renderer* renderer;
  Buffer<Vertex> clippedVertices;

 public:
  ObjectPipeline(Renderer* renderer, uint32_t maxLines)
      : renderer(renderer), clippedVertices(maxLines) {}

  void process(const Array<Object*>& objects, Camera& camera);
  void transform(const Array<Object*>& objects, Camera& camera);

 private:
  void transform(Object* object, const Matrix& viewMatrix, const Matrix& projectionMatrix);
};

}  // namespace voltage

#endif
