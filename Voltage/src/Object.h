#ifndef VOLTAGE_OBJECT_H_
#define VOLTAGE_OBJECT_H_

#include "Mesh.h"
#include "raymath.h"

namespace voltage {

enum class Culling { Front, Back, None };
enum class Shading { None, Hidden, Distance };

class Object {
 public:
  Mesh* mesh;
  Vector3 rotation, translation, scaling;
  Matrix modelMatrix;
  Culling culling;
  Shading shading;
  float brightness, hiddenBrightness;

  Object(Mesh* mesh)
      : mesh(mesh),
        culling(Culling::None),
        shading(Shading::None),
        brightness(1.0),
        hiddenBrightness(0.5) {
    setRotation(0, 0, 0);
    setTranslation(0, 0, 0);
    setScaling(1.0);
  };

  Object() : Object(nullptr) {}

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
