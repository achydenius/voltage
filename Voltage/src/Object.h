#ifndef VOLTAGE_OBJECT_H_
#define VOLTAGE_OBJECT_H_

#include "mesh.h"
#include "raymath.h"

namespace voltage {

class Object {
 public:
  Mesh* mesh;
  Vector3 rotation, translation, scaling;
  Matrix modelMatrix;

  Object(Mesh* mesh) : mesh(mesh) {
    setRotation(0, 0, 0);
    setTranslation(0, 0, 0);
    setScaling(1.0);
  };

  void setRotation(float x, float y, float z) { rotation = (Vector3){x, y, z}; }
  void setTranslation(float x, float y, float z) { translation = (Vector3){x, y, z}; }
  void setScaling(float x, float y, float z) { scaling = (Vector3){x, y, z}; }
  void setScaling(float scale) { scaling = (Vector3){scale, scale, scale}; }

  Matrix& getModelMatrix() {
    Matrix scale = MatrixScale(scaling.x, scaling.y, scaling.z);
    Matrix rotate = MatrixRotateXYZ(rotation);
    Matrix translate = MatrixTranslate(translation.x, translation.y, translation.z);
    modelMatrix = MatrixMultiply(MatrixMultiply(scale, rotate), translate);

    return modelMatrix;
  }
};

}  // namespace voltage

#endif
