#ifndef VOLTAGE_ARRAY_H_
#define VOLTAGE_ARRAY_H_

#include <Arduino.h>

#include <algorithm>
#include <initializer_list>

namespace voltage {

template <typename T>
class Array {
 protected:
  T* elements;
  const size_t capacity;

 public:
  Array(size_t capacity) : capacity(capacity) { elements = new T[capacity]; }
  Array(std::initializer_list<T> il) : Array(il.size()) {
    std::copy(il.begin(), il.end(), elements);
  }

  T& operator[](uint32_t index) const { return elements[index]; }
  size_t getCapacity() const { return capacity; }
};

template <typename T>
class Buffer : public Array<T> {
  uint32_t index;

 public:
  Buffer(size_t capacity) : Array<T>(capacity) {}

  T& operator[](uint32_t index) const { return Array<T>::elements[index]; }
  uint32_t getSize() const { return index; }
  void clear() { index = 0; }
  void push(const T& element) { Array<T>::elements[index++] = element; }
};

}  // namespace voltage

#endif
