#include "Timer.h"

using namespace voltage;

void Timer::start() { begin = micros(); }

void Timer::stop() { elapsed += micros() - begin; }

void Timer::save() {
  if (sampleIndex < sampleCount) {
    samples[sampleIndex++] = elapsed;
    elapsed = 0;
  }
}

void Timer::print() {
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
