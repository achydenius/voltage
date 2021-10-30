#ifndef VOLTAGE_CAMERA_H_
#define VOLTAGE_CAMERA_H_

#include "raymath.h"

namespace voltage {

class Camera {
 public:
  virtual Matrix& getMatrix() = 0;
};

class LookAtCamera : public Camera {
  Vector3 eye, target, up;
  float fov, aspect, near, far;
  Matrix matrix;

 public:
  LookAtCamera() : up((Vector3){0, 1.0, 0}), fov(M_PI_4), aspect(1.0), near(0.01), far(100.0){};
  LookAtCamera(const Vector3& up, float fov, float aspect, float near, float far)
      : up(up), fov(fov), aspect(aspect), near(near), far(far) {}

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
