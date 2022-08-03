#ifndef VOLTAGE_MESH_BUILDER_H_
#define VOLTAGE_MESH_BUILDER_H_

namespace voltage {

class Mesh;

namespace MeshBuilder {

Mesh* createPlane(const float size);
Mesh* createCube(const float size);
Mesh* createIcosphere(const float size, const uint32_t subdivisions);

}  // namespace MeshBuilder
}  // namespace voltage

#endif
