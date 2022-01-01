#ifndef VOLTAGE_SCANLINE_EFFECT_RENDERER_H_
#define VOLTAGE_SCANLINE_EFFECT_RENDERER_H_

#include "Engine.h"
#include "Renderer.h"

namespace voltage {

class ScanlineEffectRenderer : public Renderer {
  struct ScanlineRendererVertex {
    Vector3 world;
    Vector2 projected;
    float angle;
  };

  Engine* engine;
  Buffer<ScanlineRendererVertex> vertices;
  float planeHeight;

 public:
  ScanlineEffectRenderer(Engine* engine, uint32_t maxLines)
      : engine(engine), vertices(maxLines * 1), planeHeight(0) {}

  void setPlaneHeight(float height) { planeHeight = height; };
  void add(const Array<Object*>& objects, Camera& camera) {
    Matrix worldToViewMatrix = MatrixMultiply(camera.getViewMatrix(), camera.getProjectionMatrix());

    for (uint32_t i = 0; i < objects.getCapacity(); i++) {
      Object& object = *objects[i];

      // Transform all vertices to world coordinate space
      object.mesh->transformVertices(object.getModelMatrix());

      // Create vertices for all lines clipping XZ plane
      vertices.clear();
      for (uint32_t i = 0; i < object.mesh->edgeCount; i++) {
        Vector4& a = object.mesh->edges[i].vertices.a->transformed;
        Vector4& b = object.mesh->edges[i].vertices.b->transformed;

        if ((a.y > planeHeight && b.y < planeHeight) || (a.y < planeHeight && b.y > planeHeight)) {
          vertices.push({{a.x + (planeHeight - a.y) * (b.x - a.x) / (b.y - a.y), planeHeight,
                          a.z + (planeHeight - a.y) * (b.z - a.z) / (b.y - a.y)},
                         {0},
                         0});
        }
      }

      // Calculate centerpoint of the vertices, angle around the centerpoint and sort them
      // TODO: Can be optimized a bit with: https://stackoverflow.com/a/6989383
      float xSum = 0;
      float zSum = 0;
      for (uint32_t i = 0; i < vertices.getSize(); i++) {
        xSum += vertices[i].world.x;
        zSum += vertices[i].world.z;
      }
      Vector3 center = {xSum / vertices.getSize(), 0, zSum / vertices.getSize()};

      for (uint32_t i = 0; i < vertices.getSize(); i++) {
        ScanlineRendererVertex& vertex = vertices[i];
        vertex.angle = atan2(vertex.world.x - center.x, vertex.world.z - center.z);
      }

      qsort(vertices.getElements(), vertices.getSize(), sizeof(ScanlineRendererVertex),
            compareFunc);

      // Transform vertices to view space and perspective divide them
      for (uint32_t i = 0; i < vertices.getSize(); i++) {
        ScanlineRendererVertex& vertex = vertices[i];
        Vector4 transformed = Vector4Transform(
            {vertex.world.x, vertex.world.y, vertex.world.z, 1.0}, worldToViewMatrix);
        vertex.projected = {transformed.x / transformed.w, transformed.y / transformed.w};
      }

      // Form lines from successive vertices
      for (uint32_t i = 0; i < vertices.getSize(); i++) {
        engine->add({vertices[i].projected, vertices[(i + 1) % vertices.getSize()].projected});
      }
    }
  }

 private:
  static int compareFunc(const void* a, const void* b) {
    float diff = ((ScanlineRendererVertex*)a)->angle - ((ScanlineRendererVertex*)b)->angle;
    if (diff < 0) {
      return -1;
    } else if (diff > 0) {
      return 1;
    } else {
      return 0;
    }
  }
};

}  // namespace voltage

#endif
