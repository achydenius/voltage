#ifndef VOLTAGE_OBJECT_H_
#define VOLTAGE_OBJECT_H_

#include "mesh.h"
#include "raymath.h"

namespace voltage {

enum FaceCulling { Front, Back, None };

class Object3D {
 public:
  Mesh* mesh;
  Vector3 rotation, translation, scaling;
  Matrix modelMatrix;
  FaceCulling faceCulling;

  Object3D(Mesh* mesh) : mesh(mesh), faceCulling(None) {
    setRotation(0, 0, 0);
    setTranslation(0, 0, 0);
    setScaling(1.0);
  };

  Object3D() : Object3D(nullptr) {}

  void setRotation(float x, float y, float z) { rotation = {x, y, z}; }
  void setTranslation(float x, float y, float z) { translation = {x, y, z}; }
  void setScaling(float x, float y, float z) { scaling = {x, y, z}; }
  void setScaling(float scale) { scaling = {scale, scale, scale}; }

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
