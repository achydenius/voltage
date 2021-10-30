#ifndef VOLTAGE_CAMERA_H_
#define VOLTAGE_CAMERA_H_

#include "raymath.h"

namespace voltage {

class Camera {
 protected:
  float fov, aspect, near, far;
  Matrix matrix;

 public:
  Camera() : fov(M_PI_4), aspect(1.0), near(0.01), far(100.0) {}
  Camera(float fov, float aspect, float near, float far)
      : fov(fov), aspect(aspect), near(near), far(far) {}
  virtual Matrix& getMatrix() = 0;
};

class FreeCamera : public Camera {
  Vector3 rotation, translation;
  Matrix matrix;

 public:
  Matrix& getMatrix() {
    Matrix rotate = MatrixRotateXYZ((Vector3){-rotation.x, -rotation.y, -rotation.z});
    Matrix translate = MatrixTranslate(-translation.x, -translation.y, -translation.z);
    matrix = MatrixMultiply(MatrixMultiply(translate, rotate),
                            MatrixPerspective(fov, aspect, near, far));
    return matrix;
  }

  void setRotation(float x, float y, float z) { rotation = (Vector3){x, y, z}; }
  void setTranslation(float x, float y, float z) { translation = (Vector3){x, y, z}; }
};

class LookAtCamera : public Camera {
  Vector3 eye, target, up;

 public:
  LookAtCamera() : up((Vector3){0, 1.0, 0}){};
  LookAtCamera(const Vector3& up, float fov, float aspect, float near, float far)
      : Camera(fov, aspect, near, far), up(up) {}

  Matrix& getMatrix() {
    matrix =
        MatrixMultiply(MatrixLookAt(eye, target, up), MatrixPerspective(fov, aspect, near, far));
    return matrix;
  }

  void setEye(float x, float y, float z) { eye = (Vector3){x, y, z}; }

  void setTarget(float x, float y, float z) { target = (Vector3){x, y, z}; }
};

};  // namespace voltage

#endif
