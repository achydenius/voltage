# Voltage

An oscillographic programming library for Arduino-compatible microcontrollers. Development is currently being done with [Teensy 3.6](https://www.pjrc.com/store/teensy36.html).

The project depends on header-only math library from [raylib](https://www.raylib.com).

## How to use

1. Connect Teensy's DAC0, DAC1 and GND pins to oscilloscope's X and Y inputs
2. Download [raymath.h](https://github.com/raysan5/raylib/blob/master/src/raymath.h) header file and copy it under _Voltage/src_ directory
3. Install the library by copying the _Voltage_ directory to Arduino _libraries_ folder (e.g. _~/Documents/Arduino/libraries/_ on macOS)

## Code example

Drawing a rotating line:

```cpp
#include <Voltage.h>

voltage::Renderer renderer(12);

void setup() {}

float phase = 0;
void loop() {
  renderer.drawLine(
    { cosf(phase) * 0.75, sinf(phase) * 0.75 },
    { cosf(PI + phase) * 0.75, sinf(PI + phase) * 0.75 }
  );
  phase += 0.01;
}
```
