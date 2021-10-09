#include <Arduino.h>

void drawPoint(uint32_t x, uint32_t y) {
    analogWrite(A21, x);
    analogWrite(A22, y);
}
