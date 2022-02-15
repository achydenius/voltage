#ifndef VOLTAGE_OBJECT_H_
#define VOLTAGE_OBJECT_H_

#include "mesh.h"
#include "raymath.h"

namespace voltage {

enum FaceCulling { Front, Back, None };

class Object2D {
 public:
  Line2D* lines;
  float rotation;
  Vector2 translation, scaling;
  Matrix3 matrix;

  Object2D(Line2D* lines) : lines(lines) {}

  void setRotation(float angle) { rotation = angle; }
  void setTranslation(float x, float y) { translation = {x, y}; }
  void setScaling(float x, float y) { scaling = {x, y}; }
  void setScaling(float scale) { scaling = {scale, scale}; }

  Matrix3& getModelMatrix() {
    Matrix3 scale = Matrix3Scale(scaling.x, scaling.y);
    Matrix3 rotate = Matrix3Rotate(rotation);
    Matrix3 translate = Matrix3Translate(translation.x, translation.y);
    matrix = Matrix3Multiply(Matrix3Multiply(scale, rotate), translate);

    return matrix;
  }
};

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
