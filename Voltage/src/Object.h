#ifndef VOLTAGE_OBJECT_H_
#define VOLTAGE_OBJECT_H_

#include "mesh.h"
#include "raymath.h"

namespace voltage {

class Object {
 public:
  Mesh* mesh;
  Vector3 rotation, translation, scaling;
  Vector2* projected;

  Object(Mesh* m) : mesh(m) {
    setScaling(1.0);
    projected = new Vector2[m->vertexCount];
  };

  ~Object() { delete projected; }

  void setRotation(float x, float y, float z) { setVector(rotation, x, y, z); }
  void setTranslation(float x, float y, float z) { setVector(translation, x, y, z); }
  void setScaling(float x, float y, float z) { setVector(scaling, x, y, z); }
  void setScaling(float scale) { setVector(scaling, scale, scale, scale); }

 private:
  void setVector(Vector3& vector, float x, float y, float z) {
    vector.x = x;
    vector.y = y;
    vector.z = z;
  }
};

}  // namespace voltage

#endif
