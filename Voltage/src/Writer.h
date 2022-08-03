#ifndef VOLTAGE_WRITER_H_
#define VOLTAGE_WRITER_H_

#include <cstdint>

namespace voltage {

class SingleDACWriter {
 public:
  virtual void write(uint32_t value) const = 0;
};

class DualDACWriter {
 public:
  virtual void write(uint32_t a, uint32_t b) const = 0;
};

}  // namespace voltage

#endif
