#ifndef VOLTAGE_CAMERA_H_
#define VOLTAGE_CAMERA_H_

#include "raymath.h"

namespace voltage {

class Camera {
 protected:
  float fov, aspect, near, far;
  Matrix viewMatrix, projectionMatrix;

 public:
  Camera(float fov = M_PI_4, float aspect = 1.0, float near = 0.01, float far = 100.0)
      : fov(fov), aspect(aspect), near(near), far(far) {
    viewMatrix = MatrixIdentity();
    projectionMatrix = MatrixIdentity();
  }

  virtual Matrix& getViewMatrix() = 0;
  virtual Matrix& getProjectionMatrix() {
    projectionMatrix = MatrixPerspective(fov, aspect, near, far);
    return projectionMatrix;
  };
};

class FreeCamera : public Camera {
  Vector3 rotation = {0, 0, 0};
  Vector3 translation = {0, 0, 0};

 public:
  FreeCamera(float fov, float aspect, float near, float far) : Camera(fov, aspect, near, far) {}
  FreeCamera() : Camera() {}

  Matrix& getViewMatrix() {
    viewMatrix = MatrixMultiply(MatrixTranslate(-translation.x, -translation.y, -translation.z),
                                MatrixRotateXYZ({-rotation.x, -rotation.y, -rotation.z}));
    return viewMatrix;
  }

  void setRotation(float x, float y, float z) { rotation = {x, y, z}; }
  void setTranslation(float x, float y, float z) { translation = {x, y, z}; }
};

class LookAtCamera : public Camera {
  Vector3 eye = {0, 0, 0};
  Vector3 target = {0, 0, 0};
  Vector3 up = {0, 1.0, 0};

 public:
  LookAtCamera(float fov, float aspect, float near, float far) : Camera(fov, aspect, near, far) {}
  LookAtCamera() : Camera() {}

  Matrix& getViewMatrix() {
    viewMatrix = MatrixLookAt(eye, target, up);
    return viewMatrix;
  }

  void setEye(float x, float y, float z) { eye = {x, y, z}; }
  void setTarget(float x, float y, float z) { target = {x, y, z}; }
};

};  // namespace voltage

#endif
