#include <iostream>

#include "../Voltage/src/Writer.h"

class SDL2Writer : public voltage::DualDACWriter {
 public:
  void write(const uint32_t a, const uint32_t b) const { std::cout << a << ", " << b << std::endl; }
};
