#include "Renderer.h"

#include "Clipper.h"
#include "Engine.h"
#include "Timer.h"
#include "utils.h"

using namespace voltage;

TIMER_CREATE(transform);
TIMER_CREATE(nearClip);
TIMER_CREATE(faceCulling);

void Renderer::render(const Array<Object*>& objects, Camera& camera) {
  Matrix viewMatrix = camera.getViewMatrix();
  Matrix projectionMatrix = camera.getProjectionMatrix();

  for (uint32_t i = 0; i < objects.getCapacity(); i++) {
    render(objects[i], viewMatrix, projectionMatrix);
  }

  TIMER_SAVE(transform);
  TIMER_SAVE(nearClip);
  TIMER_SAVE(faceCulling);

  TIMER_PRINT(transform);
  TIMER_PRINT(nearClip);
  TIMER_PRINT(faceCulling);
}

void Renderer::render(Object* object, const Matrix& viewMatrix, const Matrix& projectionMatrix) {
  Mesh* mesh = object->mesh;

  // Transform camera to model space and perform face culling.
  // If culling is disabled, mark all faces and vertices visible
  TIMER_START(faceCulling);
  Matrix modelViewMatrix = MatrixMultiply(object->getModelMatrix(), viewMatrix);
  Matrix viewModelMatrix = MatrixInvert(modelViewMatrix);
  Vector3 cameraPosition = Vector3Transform({0, 0, 0}, viewModelMatrix);

  mesh->setVerticesVisible(false);

  for (uint32_t i = 0; i < mesh->faceCount; i++) {
    Face& face = mesh->faces[i];
    if (object->culling == Culling::None) {
      face.isVisible = true;
    } else {
      float angle = face.getNormalAngle(cameraPosition);
      face.isVisible = object->culling == Culling::Front ? angle < 0 : angle > 0;
    }

    for (uint32_t j = 0; j < face.edgeCount; j++) {
      face.edges[j]->vertices.a->isVisible = true;
      face.edges[j]->vertices.b->isVisible = true;
    }
  }
  TIMER_STOP(faceCulling);

  // Transform visible vertices (i.e. the ones being part of a potentially visible edge)
  TIMER_START(transform);
  Matrix modelViewProjectionMatrix = MatrixMultiply(modelViewMatrix, projectionMatrix);
  mesh->transformVisibleVertices(modelViewProjectionMatrix);
  TIMER_STOP(transform);

  // Clip lines against near plane
  // TODO: Do all clipping in clip space?
  clippedVertices.clear();

  TIMER_START(nearClip);
  for (uint32_t i = 0; i < mesh->edgeCount; i++) {
    Edge& edge = mesh->edges[i];
    Vertex* ap = edge.vertices.a;
    Vertex* bp = edge.vertices.b;

    edge.isVisible = false;

    // Define edge culling from adjacent face/faces
    // The culling infromation is needed later when rendering hidden lines with different brightness
    edge.isCulled =
        !edge.faces.a->isVisible && (edge.faces.b == nullptr || !edge.faces.b->isVisible);

    // When rendering with no shading, further processing of the edge can be skipped altogether
    if (edge.isCulled && object->shading == Shading::None) {
      continue;
    }

    Vector4 a = ap->transformed;
    Vector4 b = bp->transformed;

    ClipResult clipResult = clipLineNear(a, b);

    switch (clipResult) {
      case ClipResult::Outside:
        continue;
      case ClipResult::Inside:
        break;
      case ClipResult::AClipped:
        clippedVertices.push({{0}, a, true});
        ap = &clippedVertices.getLast();
        break;
      case ClipResult::BClipped:
        clippedVertices.push({{0}, b, true});
        bp = &clippedVertices.getLast();
        break;
    }

    ap->isVisible = true;
    bp->isVisible = true;
    edge.clipped = {ap, bp};
    edge.isVisible = true;
  }
  TIMER_STOP(nearClip);

  // Perspective divide visible both original and clipper-generated vertices
  TIMER_START(transform);
  for (uint32_t i = 0; i < mesh->vertexCount; i++) {
    Vertex& vertex = mesh->vertices[i];
    if (vertex.isVisible) {
      vertex.perspectiveDivide();
    }
  }
  for (uint32_t i = 0; i < clippedVertices.getSize(); i++) {
    clippedVertices[i].perspectiveDivide();
  }
  TIMER_STOP(transform);

  for (uint32_t i = 0; i < mesh->edgeCount; i++) {
    Edge& edge = mesh->edges[i];
    Vector4& a = edge.clipped.a->transformed;
    Vector4& b = edge.clipped.b->transformed;

    if (edge.isVisible) {
      if (object->shading == Shading::Hidden) {
        float brightness = edge.isCulled ? object->hiddenBrightness : object->brightness;
        engine->add({{a.x, a.y}, {b.x, b.y}, brightness});
      } else {
        engine->add({{a.x, a.y}, {b.x, b.y}, object->brightness});
      }
    }
  }
}
