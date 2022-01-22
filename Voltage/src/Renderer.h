#ifndef VOLTAGE_RENDERER_H_
#define VOLTAGE_RENDERER_H_

#include "Array.h"
#include "Camera.h"
#include "Object.h"
#include "types.h"

namespace voltage {

class Engine;

class Renderer {
 public:
  virtual ~Renderer(){};
  virtual void add(const Array<Object*>& objects, Camera& camera) = 0;

 protected:
  void cullFaces(Mesh* mesh, const Matrix& modelViewMatrix, const FaceCulling culling);
  void clipNear(Mesh* mesh, Buffer<Vertex>& clippedVertices);
};

class LineRenderer : public Renderer {
  Engine* engine;
  Buffer<Vertex> clippedVertices;

 public:
  LineRenderer(Engine* engine, uint32_t maxLines) : engine(engine), clippedVertices(maxLines) {}

  void add(const Array<Object*>& objects, Camera& camera);

 private:
  void add(Object* object, const Matrix& viewMatrix, const Matrix& projectionMatrix);
};

}  // namespace voltage

#endif
