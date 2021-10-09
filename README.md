# Voltage

An oscillographic programming library for Arduino-compatible microcontrollers. Development is currently being done with [Teensy 3.6](https://www.pjrc.com/store/teensy36.html).

## How to use

1. Connect Teensy's DAC0, DAC1 and GND pins to oscilloscope's X and Y inputs
2. Install the library by copying the _Voltage_ directory to Arduino _libraries_ folder (e.g. _~/Documents/Arduino/libraries/_ on macOS)

## Code example

Drawing a rotating line:

```cpp
#include <Voltage.h>

voltage::Renderer renderer(12);

void setup() {}

float phase = 0;
void loop() {
  renderer.drawLine(
    2048 + cos(phase) * 1024,
    2048 - sin(phase) * 1024,
    2048 + cos(PI + phase) * 1024,
    2048 - sin(PI + phase) * 1024
  );
  phase += 0.01;
}
```
