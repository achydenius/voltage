#include "Renderer.h"

#include "Clipper.h"
#include "Engine.h"
#include "Timer.h"
#include "utils.h"

using namespace voltage;

TIMER_CREATE(transform);
TIMER_CREATE(nearClip);
TIMER_CREATE(viewportClip);
TIMER_CREATE(faceCulling);

void LineRenderer::add(const Array<Object*>& objects, Camera& camera) {
  Matrix viewMatrix = camera.getViewMatrix();
  Matrix projectionMatrix = camera.getProjectionMatrix();

  for (uint32_t i = 0; i < objects.getCapacity(); i++) {
    add(objects[i], viewMatrix, projectionMatrix);
  }

  TIMER_SAVE(transform);
  TIMER_SAVE(nearClip);
  TIMER_SAVE(viewportClip);
  TIMER_SAVE(faceCulling);

  TIMER_PRINT(transform);
  TIMER_PRINT(nearClip);
  TIMER_PRINT(viewportClip);
  TIMER_PRINT(faceCulling);
}

void LineRenderer::add(Object* object, const Matrix& viewMatrix, const Matrix& projectionMatrix) {
  Matrix modelViewMatrix = MatrixMultiply(object->getModelMatrix(), viewMatrix);

  // Mark all vertices not visible
  TIMER_START(faceCulling);
  Mesh* mesh = object->mesh;
  for (uint32_t i = 0; i < mesh->vertexCount; i++) {
    Vector3& vertex = mesh->vertices[i];
    processedVertices[i] = {{vertex.x, vertex.y, vertex.z, 1.0}, false};
  }

  // Transform camera to model space and perform face culling
  Matrix viewModelMatrix = MatrixInvert(modelViewMatrix);
  Vector3 camera = Vector3Transform({0, 0, 0}, viewModelMatrix);

  for (uint32_t i = 0; i < mesh->faceCount; i++) {
    Face& face = mesh->faces[i];
    if (object->faceCulling == None) {
      face.isVisible = true;
    } else {
      Vector4 vertex = processedVertices[mesh->faces[i].vertexIndices[0]].vector;
      Vector3 view = Vector3Subtract(camera, {vertex.x, vertex.y, vertex.z});
      float angle = Vector3DotProduct(view, face.normal);

      if (object->faceCulling == Front) {
        face.isVisible = angle < 0;
      } else {
        face.isVisible = angle > 0;
      }
    }

    if (face.isVisible) {
      for (uint32_t j = 0; j < face.vertexCount; j++) {
        processedVertices[face.vertexIndices[j]].isVisible = true;
      }
    }
  }
  TIMER_STOP(faceCulling);

  // Transform to clip space
  TIMER_START(transform);
  Matrix modelViewProjectionMatrix = MatrixMultiply(modelViewMatrix, projectionMatrix);

  for (uint32_t i = 0; i < mesh->vertexCount; i++) {
    processedVertices[i].vector =
        Vector4Transform(processedVertices[i].vector, modelViewProjectionMatrix);
  }
  TIMER_STOP(transform);

  // Clip lines against near plane
  // TODO: Do all clipping in clip space?
  clippedVertices.clear();
  processedLines.clear();

  TIMER_START(nearClip);
  for (uint32_t i = 0; i < mesh->edgeCount; i++) {
    Vertex* ap = &processedVertices[mesh->edges[i].vertexIndices.a];
    Vertex* bp = &processedVertices[mesh->edges[i].vertexIndices.b];

    if (!ap->isVisible || !bp->isVisible) {
      continue;
    }

    Vector4 a = ap->vector;
    Vector4 b = bp->vector;

    ClipResult clipResult = clipLineNear(a, b);

    switch (clipResult) {
      case Outside:
        ap->isVisible = false;
        bp->isVisible = false;
        continue;
      case Inside:
        break;
      case AClipped:
        clippedVertices.push((Vertex){a, true});
        ap = &clippedVertices.getLast();
        break;
      case BClipped:
        clippedVertices.push((Vertex){b, true});
        bp = &clippedVertices.getLast();
        break;
    }
    if (object->faceCulling != None ||
        (mesh->edges[i].faces.a->isVisible || mesh->edges[i].faces.b->isVisible)) {
      processedLines.push((Line<Vertex*>){ap, bp});
    }
  }
  TIMER_STOP(nearClip);

  uint32_t visibleVerticesCount = 0;

  // Project vertices
  TIMER_START(transform);
  for (uint32_t i = 0; i < mesh->vertexCount; i++) {
    Vertex& vertex = processedVertices[i];
    if (vertex.isVisible) {
      vertex.vector.x /= vertex.vector.w;
      vertex.vector.y /= vertex.vector.w;
      visibleVerticesCount++;
    }
  }
  for (uint32_t i = 0; i < clippedVertices.getSize(); i++) {
    Vertex& vertex = clippedVertices[i];
    vertex.vector.x /= vertex.vector.w;
    vertex.vector.y /= vertex.vector.w;
    visibleVerticesCount++;
  }
  TIMER_STOP(transform);

  for (uint32_t i = 0; i < processedLines.getSize(); i++) {
    Vertex* a = processedLines[i].a;
    Vertex* b = processedLines[i].b;

    TIMER_START(viewportClip);
    engine->add((Line2D){(Vector2){a->vector.x, a->vector.y}, (Vector2){b->vector.x, b->vector.y}});
    TIMER_STOP(viewportClip);
  }
}
