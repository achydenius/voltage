#ifndef VOLTAGE_DAC_WRITER_
#define VOLTAGE_DAC_WRITER_

// Use Teensy 3.6 core library definitions
#ifndef __MK66FX1M0__
#define __MK66FX1M0__
#endif
#include <Arduino.h>
#include <SPI.h>

#include "Writer.h"

namespace voltage {

typedef int16_t __attribute__((__may_alias__)) aliased_int16_t;

class Teensy36Writer : public DualDACWriter {
 public:
  uint32_t getMaxValue() const { return 4095; }

  // Low-level analog write implementation for Teensy 3.6 is copy-pasted here from the core library:
  // https://github.com/PaulStoffregen/cores/blob/1.54/teensy3/analog.c#L520-L564
  // This dramatically improves performance as the function calls in rendering loop are bypassed.
  inline void write(uint32_t a, uint32_t b) const {
    SIM_SCGC2 |= SIM_SCGC2_DAC0;
    DAC0_C0 = DAC_C0_DACEN | DAC_C0_DACRFS;  // 3.3V VDDA is DACREF_2
    *(volatile aliased_int16_t *)&(DAC0_DAT0L) = a;

    SIM_SCGC2 |= SIM_SCGC2_DAC1;
    DAC1_C0 = DAC_C0_DACEN | DAC_C0_DACRFS;  // 3.3V VDDA is DACREF_2
    *(volatile aliased_int16_t *)&(DAC1_DAT0L) = b;
  }
};

class MCP4922Writer : public SingleDACWriter, public DualDACWriter {
  static const uint16_t channel1Mask = 0b0111000000000000;
  static const uint16_t channel2Mask = 0b1111000000000000;
  static const uint32_t transferSpeed = 20000000;
  const uint32_t selectPin;

 public:
  MCP4922Writer(uint32_t selectPin = 10) : selectPin(selectPin) {
    pinMode(selectPin, OUTPUT);
    SPI.begin();
  }

  inline uint32_t getMaxValue() const { return 4095; }

  inline void write(uint32_t value) const {
    SPI.beginTransaction(SPISettings(transferSpeed, MSBFIRST, SPI_MODE0));

    digitalWriteFast(selectPin, LOW);
    SPI.transfer16(channel1Mask | value);
    digitalWriteFast(selectPin, HIGH);
    __asm("nop");

    SPI.endTransaction();
  }

  inline void write(uint32_t a, uint32_t b) const {
    SPI.beginTransaction(SPISettings(transferSpeed, MSBFIRST, SPI_MODE0));

    digitalWriteFast(selectPin, LOW);
    SPI.transfer16(channel1Mask | a);
    digitalWriteFast(selectPin, HIGH);
    __asm__("nop");

    digitalWriteFast(selectPin, LOW);
    SPI.transfer16(channel2Mask | b);
    digitalWriteFast(selectPin, HIGH);
    __asm__("nop");

    SPI.endTransaction();
  }
};

}  // namespace voltage

#endif
