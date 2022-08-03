// Uncomment for enabling performance profiling
// #define VOLTAGE_PROFILE_SAMPLES 100

#ifndef VOLTAGE_TIMER_H_
#define VOLTAGE_TIMER_H_

#ifndef VOLTAGE_EMULATOR
#include <Arduino.h>
#endif

#include <string>

#ifdef VOLTAGE_PROFILE_SAMPLES
#define TIMER_CREATE(name) static Timer _timer_##name(#name, VOLTAGE_PROFILE_SAMPLES)
#define TIMER_START(name) _timer_##name.start()
#define TIMER_STOP(name) _timer_##name.stop()
#define TIMER_SAVE(name) _timer_##name.save()
#define TIMER_PRINT(name) _timer_##name.print()
#else
#define TIMER_CREATE(name)
#define TIMER_START(name)
#define TIMER_STOP(name)
#define TIMER_SAVE(name)
#define TIMER_PRINT(name)
#endif

namespace voltage {

class Timer {
  const char *name;
  const size_t sampleCount;
  uint64_t *samples;
  uint32_t sampleIndex;
  uint64_t begin;
  uint64_t elapsed;
  bool isPrinted;

 public:
  Timer(const char *name, const size_t sampleCount)
      : name(name), sampleCount(sampleCount), sampleIndex(0), elapsed(0), isPrinted(false) {
    samples = new uint64_t[sampleCount];
  }
  ~Timer() { delete samples; }

  void start();
  void stop();
  void save();
  void print();
};

}  // namespace voltage

#endif
