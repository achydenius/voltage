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
  Array<float> heights;

 public:
  ScanlineEffectRenderer(Engine* engine, uint32_t maxLines, Array<float> heights)
      : engine(engine), vertices(maxLines * 1), heights(heights) {}

  // void setPlaneHeights(Array<float> heights) { this->heights = heights; };
  void add(const Array<Object*>& objects, Camera& camera) {
    Matrix viewMatrix = camera.getViewMatrix();
    Matrix projectionMatrix = camera.getProjectionMatrix();

    for (uint32_t i = 0; i < objects.getCapacity(); i++) {
      add(objects[i], viewMatrix, projectionMatrix);
    }
  }

 private:
  void add(Object* object, const Matrix& viewMatrix, const Matrix& projectionMatrix) {
    Mesh* mesh = object->mesh;

    // Cull faces in model coordinate space
    Matrix modelViewMatrix = MatrixMultiply(object->getModelMatrix(), viewMatrix);
    if (object->faceCulling != None) {
      cullFaces(mesh, modelViewMatrix, object->faceCulling);
    } else {
      mesh->setVerticesVisible(true);
    }

    // Transform all vertices to world coordinate space
    mesh->transformVisibleVertices(object->getModelMatrix());

    // Create vertices for all visible lines clipping XZ plane
    for (uint32_t i = 0; i < heights.getCapacity(); i++) {
      vertices.clear();
      for (uint32_t j = 0; j < mesh->edgeCount; j++) {
        Edge& edge = mesh->edges[j];
        Vertex* ap = edge.vertices.a;
        Vertex* bp = edge.vertices.b;

        if (ap->isVisible && bp->isVisible) {
          Vector4& a = edge.vertices.a->transformed;
          Vector4& b = edge.vertices.b->transformed;

          float height = heights[i];
          if ((a.y > height && b.y < height) || (a.y < height && b.y > height)) {
            vertices.push({{a.x + (height - a.y) * (b.x - a.x) / (b.y - a.y), height,
                            a.z + (height - a.y) * (b.z - a.z) / (b.y - a.y)},
                           {0},
                           0});
          }
        }
      }

      // Calculate centerpoint of the vertices, angle around the centerpoint and sort them
      // TODO: Can be optimized a bit with: https://stackoverflow.com/a/6989383
      float xSum = 0;
      float zSum = 0;
      for (uint32_t j = 0; j < vertices.getSize(); j++) {
        xSum += vertices[j].world.x;
        zSum += vertices[j].world.z;
      }
      Vector3 center = {xSum / vertices.getSize(), 0, zSum / vertices.getSize()};

      for (uint32_t j = 0; j < vertices.getSize(); j++) {
        ScanlineRendererVertex& vertex = vertices[j];
        vertex.angle = atan2(vertex.world.x - center.x, vertex.world.z - center.z);
      }

      qsort(vertices.getElements(), vertices.getSize(), sizeof(ScanlineRendererVertex),
            compareFunc);

      // Transform vertices to view space and perspective divide them
      Matrix worldToViewMatrix = MatrixMultiply(viewMatrix, projectionMatrix);

      for (uint32_t j = 0; j < vertices.getSize(); j++) {
        ScanlineRendererVertex& vertex = vertices[j];
        Vector4 transformed = Vector4Transform(
            {vertex.world.x, vertex.world.y, vertex.world.z, 1.0}, worldToViewMatrix);
        vertex.projected = {transformed.x / transformed.w, transformed.y / transformed.w};
      }

      // Form lines from successive vertices
      if (vertices.getSize() >= 2) {
        for (uint32_t j = 0; j < vertices.getSize() - 1; j++) {
          engine->add({vertices[j].projected, vertices[j + 1].projected});
        }
      }
    }
  }

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
