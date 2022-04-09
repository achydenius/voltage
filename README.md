# Voltage

An oscillographics programming library for [Teensy 3.6](https://www.pjrc.com/store/teensy36.html). The goal of the library is to provide a simple to use API for creating three-dimensional vector graphics similar to classic arcade games such as [Battlezone](https://www.arcade-museum.com/game_detail.php?game_id=7059) or [Star Wars](https://www.arcade-museum.com/game_detail.php?game_id=9773). 

The project depends on [raylib's](https://www.raylib.com) header-only math library. An external DAC can be used to control the brightness of individual lines.

![An oscilloscope running a Voltage example](https://raw.githubusercontent.com/achydenius/voltage/main/three-cubes.jpg)

## Setting up

1. Connect Teensy's DAC0, DAC1 and GND pins to oscilloscope's X and Y inputs
2. Download [raymath.h](https://github.com/raysan5/raylib/blob/master/src/raymath.h) header file and copy it under _Voltage/src_ directory
3. Install the library by copying the _Voltage_ directory to Arduino _libraries_ folder (e.g. _~/Documents/Arduino/libraries/_ on macOS)

## How to use

The rendering loop consists of three phases:

1. Clear `Renderer`'s internal buffers with `clear` method call
2. Add geometry to be rendered with the overloaded `add` method
3. Render all the added geometry with `render` method call

The preferred rendering resolution is defined when instantiating the renderer. Usually values from 10 to 12 (Teensy's maximum resolution) seem to work nicely. Higher resolution produces a smoother result but requires more CPU power, thus reducing the amount of primitives that can be rendered without flickering.

## Display calibration

Connected display can be adjusted (e.g. setting correct X/Y/Z scaling) a bit easier by calling `Renderer`'s `renderCalibrationView` method. Calibration view shows a cube for adjusting the aspect ratio, crosshair for centering the image and corner markers for displaying the current clipping viewport. The desired viewport can be set with `Renderer`'s `setViewport` method.

## Setting up external DAC for brightness control

An external [Microchip MCP4922](https://www.microchip.com/en-us/product/MCP4922) DAC can be used for setting the brightness of individual lines. MCP4922 can be used with Teensy 3.6 by using the following connections:

| MCP4922                   | Teensy 3.6     |
|---------------------------|----------------|
| Pin 1 (V<sub>DD</sub>)    | 3.3V           |
| Pin 3 (CS)                | Pin 10 (CS0)   |
| Pin 4 (SCK)               | Pin 13 (SCK0)  |
| Pin 5 (SDI)               | Pin 11 (MOSI0) |
| Pin 8 (LDAC)              | GND            |
| Pin 9 (SHDN)              | 3.3V           |
| Pin 12 (V<sub>SS</sub>)   | GND            |
| Pin 13 (V<sub>REFA</sub>) | 3.3V           |

The DAC output voltage is in pin 14 (V<sub>OUTA</sub>).

## Code examples

More examples can be found in [examples](examples/) directory.

### Draw a rotating line by passing a `Line` to `add` method:

```cpp
#include <Voltage.h>

voltage::Renderer renderer(12);

void setup() {}

float phase = 0;
void loop() {
  renderer.clear();
  renderer.add({
    { cosf(phase), sinf(phase) },
    { cosf(PI + phase), sinf(PI + phase) }
  });
  renderer.render();

  phase -= 0.001;
}
```

### Draw an animated 3D cube using `Object` and `FreeCamera`:

```cpp
#include <Voltage.h>

using namespace voltage;

Renderer renderer(10);
Mesh *mesh = MeshBuilder::createCube(1.0);
Object *object = new Object(mesh);
FreeCamera camera;

void setup() {}

float phase = 0;
void loop() {
  camera.setTranslation(0, 0, sin(phase) * 5.0);
  object->setRotation(phase, phase, 0);

  renderer.clear();
  renderer.add(object, camera);
  renderer.render();
  phase += 0.001;
}
```

### Initializing a MCP4922 DAC and enabling shading

Initialize the `Renderer` class as follows:

```cpp
MCP4922Writer *brightnessWriter = new MCP4922Writer();
Renderer renderer(12, brightnessWriter);
```

Enable back face culling (i.e. determining of hidden lines), enable hidden line shading and set hidden line brightness:

```cpp
void setup() {
  object->culling = Culling::Back;
  object->shading = Shading::Hidden;
  object->hiddenBrightness = 0.5;
}
```

The rest of the code works just as in previous examples.
