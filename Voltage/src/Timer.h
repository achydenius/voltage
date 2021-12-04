#ifndef VOLTAGE_TIMER_H_
#define VOLTAGE_TIMER_H_

#include <Arduino.h>

#include <string>

#ifdef VOLTAGE_PROFILE
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

  void start() { begin = micros(); }
  void stop() { elapsed += micros() - begin; }
  void save() {
    if (sampleIndex < sampleCount) {
      samples[sampleIndex++] = elapsed;
      elapsed = 0;
    }
  }
  void print() {
    if (!isPrinted && sampleIndex == sampleCount) {
      uint64_t sum = 0;
      for (uint32_t i = 0; i < sampleCount; i++) {
        sum += samples[i];
      }
      double avg = sum / (double)sampleCount;

      Serial.print(name);
      Serial.print(": ");
      Serial.print(avg / 1000);
      Serial.println(" ms");

      isPrinted = true;
    }
  }
};

}  // namespace voltage

#endif
