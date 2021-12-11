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
  Array(const size_t capacity) : capacity(capacity) { elements = new T[capacity]; }
  Array(const std::initializer_list<T> il) : Array(il.size()) {
    std::copy(il.begin(), il.end(), elements);
  }

  T& operator[](const int index) const { return elements[index]; }
  size_t getCapacity() const { return capacity; }
};

template <typename T>
class Buffer : public Array<T> {
  uint32_t index;

 public:
  Buffer(const size_t capacity) : Array<T>(capacity), index(0) {}

  T& operator[](const int index) const { return Array<T>::elements[index]; }
  uint32_t getSize() const { return index; }
  void clear() { index = 0; }
  void push(const T& element) { Array<T>::elements[index++] = element; }
  T& getLast() { return Array<T>::elements[index - 1]; }
};

}  // namespace voltage

#endif
