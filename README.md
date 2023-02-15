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

The rendering resolution is always 12 bits, which is Teensy's maximum resolution.

## Importing 3D meshes from third-party software

3D meshes in [.obj file format](https://en.wikipedia.org/wiki/Wavefront_.obj_file) can be imported to Voltage with `parse-obj.py` Python script in *utils* directory. The script takes two command line arguments: the name of the obj file to be imported, and a name for a variable, which can be then accessed in Voltage code.

For example, running `./parse-obj.py example.obj mesh` outputs code with the mesh definition in `voltage::Mesh* mesh` variable, which can be then pasted to the sketch. See [import.ino](examples/import.ino) for an example.

Alternatively, the mesh can be imported by first redirecting the output of the parser to a file (e.g. with `./parse-obj.py example.obj > example.h`), copying the file to the sketch's directory, and then including the file in the sketch with `#include "example.h`.

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

voltage::Renderer renderer();

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

Renderer renderer();
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
BrightnessTransform *brightnessTransform = new LinearBrightnessTransform();
Renderer renderer(brightnessWriter, brightnessTransform);
```

Enable hidden line shading and set the hidden line brightness:

```cpp
void setup() {
  object->shading = Shading::Hidden;
  object->hiddenBrightness = 0.5;
}
```

The rest of the code works just as in previous examples.

## Running without oscilloscope and Teensy on MacOS (experimental)

Voltage can also be used without oscilloscope and Teensy. This can be useful for a bit more convenient testing and development. The oscilloscope/Teensy emulator can be found in *emulator* directory. *main.cpp* file includes the cube example above and contains further instructions how to modify/use the code.

1. Install [SDL2](https://www.libsdl.org/) with `brew install sdl2`
2. Build emulator with `make`
3. Run the emulator with `./main`
