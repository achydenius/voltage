#ifndef VOLTAGE_CAMERA_H_
#define VOLTAGE_CAMERA_H_

#include "raymath.h"

namespace voltage {

class Camera {
 protected:
  float fov, aspect, near, far;
  Matrix matrix;

 public:
  Camera(float fov, float aspect, float near, float far)
      : fov(fov), aspect(aspect), near(near), far(far) {
    matrix = MatrixIdentity();
  }
  Camera() : Camera(M_PI_4, 1.0, 0.01, 100.0) {}
  virtual Matrix& getMatrix() = 0;
};

class FreeCamera : public Camera {
  Vector3 rotation = (Vector3){0, 0, 0};
  Vector3 translation = (Vector3){0, 0, 0};

 public:
  FreeCamera(float fov, float aspect, float near, float far) : Camera(fov, aspect, near, far) {}
  FreeCamera() : Camera() {}

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
  Vector3 eye = (Vector3){0, 0, 0};
  Vector3 target = (Vector3){0, 0, 0};
  Vector3 up = (Vector3){0, 1.0, 0};

 public:
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
