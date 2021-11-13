# Voltage

An oscillographics programming library for Arduino-compatible microcontrollers. Development is currently being done with [Teensy 3.6](https://www.pjrc.com/store/teensy36.html).

The project depends on [raylib's](https://www.raylib.com) header-only math library.

## How to use

1. Connect Teensy's DAC0, DAC1 and GND pins to oscilloscope's X and Y inputs
2. Download [raymath.h](https://github.com/raysan5/raylib/blob/master/src/raymath.h) header file and copy it under _Voltage/src_ directory
3. Install the library by copying the _Voltage_ directory to Arduino _libraries_ folder (e.g. _~/Documents/Arduino/libraries/_ on macOS)

## Code examples

### Draw two rotating lines using `addLine` method:

```cpp
#include <Voltage.h>

voltage::Engine engine(10);

voltage::Line2D createLine(float angle, float length) {
  return {
    { cosf(angle) * length, sinf(angle) * length },
    { cosf(PI + angle) * length, sinf(PI + angle) * length }
  };
}

void setup() {}

float phase = 0;
void loop() {
  engine.clear();
  engine.addLine(createLine(phase, 0.75));
  engine.addLine(createLine(phase + HALF_PI, 0.75));
  engine.render();

  phase -= 0.001;
}
```

### Draw an animated 3D cube using `Object` and `FreeCamera`:

```cpp
#include <Voltage.h>

voltage::Engine engine(10);
voltage::Mesh *mesh = voltage::MeshBuilder::createCube(1.0);
voltage::Array<voltage::Object*> objects({ new voltage::Object(mesh) });
voltage::FreeCamera camera;

void setup() {
  camera.setTranslation(0, 0, 5.0);
}

float phase = 0;
void loop() {
  objects[0]->setRotation(phase, phase, 0);

  engine.clear();
  engine.addObjects(objects, camera);
  engine.render();
  phase += 0.001;
}
```
