#ifndef VOLTAGE_TEENSY_3_6_RASTERIZER_H_
#define VOLTAGE_TEENSY_3_6_RASTERIZER_H_

// Use Teensy 3.6 core library definitions
#ifndef __MK66FX1M0__
#define __MK66FX1M0__
#endif
#include <Arduino.h>

#include "Rasterizer.h"

namespace voltage {

// Low-level analog write implementation for Teensy 3.6 is copy-pasted here from the core library:
// https://github.com/PaulStoffregen/cores/blob/1.54/teensy3/analog.c#L520-L564
// This improves performance dramatically as the function calls in rendering loop are bypassed.

typedef int16_t __attribute__((__may_alias__)) aliased_int16_t;

class Teensy36Rasterizer : public Rasterizer {
  Teensy36Rasterizer(uint8_t resolutionBits) : Rasterizer(resolutionBits) {}

  void dacWrite(uint32_t x, uint32_t y) const {
    SIM_SCGC2 |= SIM_SCGC2_DAC0;
    DAC0_C0 = DAC_C0_DACEN | DAC_C0_DACRFS;  // 3.3V VDDA is DACREF_2
    *(volatile aliased_int16_t *)&(DAC0_DAT0L) = x << scaleBits;

    SIM_SCGC2 |= SIM_SCGC2_DAC1;
    DAC1_C0 = DAC_C0_DACEN | DAC_C0_DACRFS;  // 3.3V VDDA is DACREF_2
    *(volatile aliased_int16_t *)&(DAC1_DAT0L) = y << scaleBits;
  }
};

}  // namespace voltage

#endif
