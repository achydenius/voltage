#ifndef VOLTAGE_EMULATOR
#include <Arduino.h>
#endif

#include "Clipper.h"
#include "ObjectPipeline.h"
#include "Renderer.h"
#include "Timer.h"
#include "utils.h"

using namespace voltage;

TIMER_CREATE(transform);
TIMER_CREATE(nearClip);
TIMER_CREATE(faceCulling);

void ObjectPipeline::process(const Array<Object*>& objects, Camera& camera) {
  Matrix viewMatrix = camera.getViewMatrix();
  Matrix projectionMatrix = camera.getProjectionMatrix();

  for (uint32_t i = 0; i < objects.getCapacity(); i++) {
    process(objects[i], viewMatrix, projectionMatrix);
  }

  TIMER_SAVE(transform);
  TIMER_SAVE(nearClip);
  TIMER_SAVE(faceCulling);

  TIMER_PRINT(transform);
  TIMER_PRINT(nearClip);
  TIMER_PRINT(faceCulling);
}

void ObjectPipeline::process(Object* object, const Matrix& viewMatrix,
                             const Matrix& projectionMatrix) {
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
    if (object->culling == Culling::Front || object->culling == Culling::Back) {
      float angle = face.getNormalAngle(cameraPosition);
      face.isVisible = object->culling == Culling::Front ? angle < 0 : angle > 0;
    } else if (object->shading == Shading::Hidden) {
      face.isVisible = face.getNormalAngle(cameraPosition) > 0;
    } else {
      face.isVisible = true;
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

  // Clip lines against camera near and far planes
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

    if (object->culling != Culling::None && edge.isCulled) {
      continue;
    }

    Vector4 a = ap->transformed;
    Vector4 b = bp->transformed;

    ClipResult clipResult = clipLineNearAndFar(a, b);

    if (clipResult == ClipResult::Outside) {
      continue;
    }
    if (clipResult == ClipResult::AClipped || clipResult == ClipResult::BothClipped) {
      clippedVertices.push({{0}, a, true});
      ap = &clippedVertices.getLast();
    }
    if (clipResult == ClipResult::BClipped || clipResult == ClipResult::BothClipped) {
      clippedVertices.push({{0}, b, true});
      bp = &clippedVertices.getLast();
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

  // Add processed lines to render buffer
  for (uint32_t i = 0; i < mesh->edgeCount; i++) {
    Edge& edge = mesh->edges[i];
    Vector4& a = edge.clipped.a->transformed;
    Vector4& b = edge.clipped.b->transformed;

    if (edge.isVisible) {
      if (object->shading == Shading::Hidden) {
        float brightness = edge.isCulled ? object->hiddenBrightness : object->brightness;
        renderer->add({{a.x, a.y}, {b.x, b.y}, brightness});
      } else {
        renderer->add({{a.x, a.y}, {b.x, b.y}, object->brightness});
      }
    }
  }
}
