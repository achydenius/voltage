#ifndef VOLTAGE_TRANSFORM_3D_H_
#define VOLTAGE_TRANSFORM_3D_H_

#include "Array.h"
#include "Camera.h"
#include "Object.h"
#include "types.h"

namespace voltage {

class Renderer;

class Transform3D {
  Renderer* renderer;
  Buffer<Vertex> clippedVertices;

 public:
  Transform3D(Renderer* renderer, uint32_t maxLines)
      : renderer(renderer), clippedVertices(maxLines) {}

  void transform(const Array<Object*>& objects, Camera& camera);

 private:
  void transform(Object* object, const Matrix& viewMatrix, const Matrix& projectionMatrix);
};

}  // namespace voltage

#endif
