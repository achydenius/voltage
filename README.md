# Voltage

An oscillographic programming library for Arduino-compatible microcontrollers. Development is currently being done with [Teensy 3.6](https://www.pjrc.com/store/teensy36.html).

The project depends on header-only math library from [raylib](https://www.raylib.com).

## How to use

1. Connect Teensy's DAC0, DAC1 and GND pins to oscilloscope's X and Y inputs
2. Download [raymath.h](https://github.com/raysan5/raylib/blob/master/src/raymath.h) header file and copy it under _Voltage/src_ directory
3. Install the library by copying the _Voltage_ directory to Arduino _libraries_ folder (e.g. _~/Documents/Arduino/libraries/_ on macOS)

## Code example

Drawing two rotating lines:

```cpp
#include <Voltage.h>

voltage::Engine engine(10);

voltage::Line createLine(float angle, float length) {
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
