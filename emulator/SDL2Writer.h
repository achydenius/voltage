#include <iostream>

#include "../Voltage/src/Writer.h"

class SDL2Writer : public voltage::DualDACWriter {
  const unsigned int resolution;
  const unsigned int shift;
  unsigned int *buffer;

 public:
  SDL2Writer(const unsigned int resolution, unsigned int *buffer)
      : resolution(resolution), shift(12 - resolution), buffer(buffer) {}

  void write(const uint32_t x, const uint32_t y) const {
    buffer[(y >> shift) * (1 << resolution) + (x >> shift)] = 0xFFFFFFFF;
  }
};
