#include <Voltage.h>

using namespace voltage;

// The mesh definition has been imported from example.obj in the current directory
Vector3 meshVertices[] = {
    {-0.5, -0.5, 0.5},  {0.5, -0.5, 0.5},  {-0.5, 0.5, 0.5},   {0.5, 0.5, 0.5},
    {-0.5, 0.5, -0.5},  {0.5, 0.5, -0.5},  {-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5},
    {3.5, -0.5, -0.5},  {3.5, -0.5, 0.5},  {3.5, 0.5, -0.5},   {3.5, 0.5, 0.5},
    {-0.5, 3.5, 0.5},   {0.5, 3.5, 0.5},   {0.5, 3.5, -0.5},   {-0.5, 3.5, -0.5},
    {-0.5, -0.5, 3.5},  {0.5, -0.5, 3.5},  {0.5, 0.5, 3.5},    {-0.5, 0.5, 3.5},
    {-3.5, -0.5, -0.5}, {-3.5, -0.5, 0.5}, {-3.5, 0.5, 0.5},   {-3.5, 0.5, -0.5},
    {-0.5, 0.5, -3.5},  {0.5, 0.5, -3.5},  {0.5, -0.5, -3.5},  {-0.5, -0.5, -3.5},
    {-0.5, -3.5, -0.5}, {0.5, -3.5, -0.5}, {0.5, -3.5, 0.5},   {-0.5, -3.5, 0.5}};
voltage::FaceDefinition meshFaces[] = {
    {16, 17, 18, 19}, {12, 13, 14, 15}, {24, 25, 26, 27}, {28, 29, 30, 31}, {9, 8, 10, 11},
    {20, 21, 22, 23}, {1, 7, 8, 9},     {7, 5, 10, 8},    {5, 3, 11, 10},   {3, 1, 9, 11},
    {2, 3, 13, 12},   {3, 5, 14, 13},   {5, 4, 15, 14},   {4, 2, 12, 15},   {0, 1, 17, 16},
    {1, 3, 18, 17},   {3, 2, 19, 18},   {2, 0, 16, 19},   {6, 0, 21, 20},   {0, 2, 22, 21},
    {2, 4, 23, 22},   {4, 6, 20, 23},   {4, 5, 25, 24},   {5, 7, 26, 25},   {7, 6, 27, 26},
    {6, 4, 24, 27},   {6, 7, 29, 28},   {7, 1, 30, 29},   {1, 0, 31, 30},   {0, 6, 28, 31}};
voltage::Mesh* mesh = new voltage::Mesh(meshVertices, 32, meshFaces, 30);

Renderer renderer(10);
Object* object = new Object(mesh);
LookAtCamera camera;

float phase = 0;
void setup() { camera.setTarget(0, 0, 0); }

void loop() {
  camera.setEye(sinf(phase) * 20.0, sinf(phase) * 5.0, 10.0);

  renderer.clear();
  renderer.add(object, camera);
  renderer.render();
  phase += 0.01;
}