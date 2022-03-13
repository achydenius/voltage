#ifndef VOLTAGE_DAC_WRITER_
#define VOLTAGE_DAC_WRITER_

// Use Teensy 3.6 core library definitions
#ifndef __MK66FX1M0__
#define __MK66FX1M0__
#endif
#include <Arduino.h>

namespace voltage {

class SingleDACWriter {
 public:
  virtual void write(uint32_t value) const = 0;
};

class DualDACWriter {
 public:
  virtual void write(uint32_t a, uint32_t b) const = 0;
};

typedef int16_t __attribute__((__may_alias__)) aliased_int16_t;

class Teensy36Writer : public DualDACWriter {
  // Low-level analog write implementation for Teensy 3.6 is copy-pasted here from the core library:
  // https://github.com/PaulStoffregen/cores/blob/1.54/teensy3/analog.c#L520-L564
  // This dramatically improves performance as the function calls in rendering loop are bypassed.
  void write(uint32_t a, uint32_t b) const {
    SIM_SCGC2 |= SIM_SCGC2_DAC0;
    DAC0_C0 = DAC_C0_DACEN | DAC_C0_DACRFS;  // 3.3V VDDA is DACREF_2
    *(volatile aliased_int16_t *)&(DAC0_DAT0L) = a;

    SIM_SCGC2 |= SIM_SCGC2_DAC1;
    DAC1_C0 = DAC_C0_DACEN | DAC_C0_DACRFS;  // 3.3V VDDA is DACREF_2
    *(volatile aliased_int16_t *)&(DAC1_DAT0L) = b;
  }
};

}  // namespace voltage

#endif
