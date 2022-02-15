# Voltage

An oscillographics programming library for Arduino-compatible microcontrollers. The goal of the library is to provide a simple to use API for creating three-dimensional vector graphics similar to classic arcade games such as [Battlezone](https://www.arcade-museum.com/game_detail.php?game_id=7059) or [Star Wars](https://www.arcade-museum.com/game_detail.php?game_id=9773).

Development is currently being done with [Teensy 3.6](https://www.pjrc.com/store/teensy36.html). The project depends on [raylib's](https://www.raylib.com) header-only math library.

![An oscilloscope running a Voltage example](https://raw.githubusercontent.com/achydenius/voltage/main/three-cubes.jpg)

## Setting up

1. Connect Teensy's DAC0, DAC1 and GND pins to oscilloscope's X and Y inputs
2. Download [raymath.h](https://github.com/raysan5/raylib/blob/master/src/raymath.h) header file and copy it under _Voltage/src_ directory
3. Install the library by copying the _Voltage_ directory to Arduino _libraries_ folder (e.g. _~/Documents/Arduino/libraries/_ on macOS)

## How to use

The rendering loop consists of three phases:

1. Clear `Engine`'s internal buffers with `clear` method call
2. Add geometry to be rendered with `addLine`, `addPoint` and `addObject3D` methods
3. Render all the added geometry with `render` method call

The preferred rendering resolution is defined when instantiating the engine. Usually values from 10 to 12 (Teensy's maximum resolution) seem to work nicely. Higher resolution produces a smoother result but requires more CPU power, thus reducing the amount of primitives that can be rendered without flickering.

## Code examples

More examples can be found in [examples](examples/) directory.

### Draw a rotating line by passing a `Line` to `add` method:

```cpp
#include <Voltage.h>

voltage::Engine engine(12);

void setup() {}

float phase = 0;
void loop() {
  engine.clear();
  engine.add({
    { cosf(phase) * 0.75, sinf(phase) * 0.75 },
    { cosf(PI + phase) * 0.75, sinf(PI + phase) * 0.75 }
  });
  engine.render();

  phase -= 0.001;
}
```

### Draw an animated 3D cube using `Object3D` and `FreeCamera`:

```cpp
#include <Voltage.h>

using namespace voltage;

Engine engine(10);
Mesh *mesh = MeshBuilder::createCube(1.0);
Object3D *object = new Object3D(mesh);
FreeCamera camera;

void setup() {
  camera.setTranslation(0, 0, 5.0);
}

float phase = 0;
void loop() {
  object->setRotation(phase, phase, 0);

  engine.clear();
  engine.add(object, camera);
  engine.render();
  phase += 0.001;
}
```
