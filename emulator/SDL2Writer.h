#include <iostream>

#include "../Voltage/src/Writer.h"

class SDL2Writer : public voltage::DualDACWriter {
  const unsigned int resolution;
  unsigned int *buffer;

 public:
  SDL2Writer(const unsigned int resolution, unsigned int *buffer)
      : resolution(resolution), buffer(buffer) {}

  uint32_t getMaxValue() const { return resolution - 1; }

  void write(const uint32_t x, const uint32_t y) const { buffer[y * resolution + x] = 0xFFFFFFFF; }
};
