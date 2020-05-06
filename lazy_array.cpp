//
// Bad idea credit: @LeonMontealegre
//
// Purpose: An in-place array that works on datatypes without default constructors 
//           that can be used on the stack or the heap.
//
#include <iostream>
#include <cstring>
#include <cassert>

template<size_t N, typename T>
class LazyArray {
private:
  char _data[N * sizeof(T)];

  const T& index(size_t i) const { return data()[i]; }
  T& index(size_t i) { return data()[i]; }
public:
  LazyArray() = default;

  const T* data() const { return (T*) _data; }
  T* data() { return (T*) _data; }

  const T& operator[](size_t i) const { return index(i); }
  T& operator[](size_t i) { return index(i); }

  T& construct_element(size_t i) {
    return *(new (&index(i)) T());
  }
  template<typename... args>
  T& construct_element(size_t i, args... a) {
    return *(new (&index(i)) T(a...));
  }

  void destruct_element(size_t i) {
    (&index(i))->~T();
  }
};

template<size_t N, typename T>
class CheckedLazyArray {
private:
  LazyArray<N,T> _data;
  uint8_t allocd[(N >> 3) + 1];

  void set_allocd(size_t i, bool v) {
    const uint8_t bit_pos = (uint8_t)1 << (i & 3);
    const size_t byte_idx = i >> 3;
    allocd[byte_idx] = ((uint8_t)v * bit_pos) | (allocd[byte_idx] & (~bit_pos));
  }
  void check_exists(size_t i) { assert(exists(i)); }
  void check_not_exists(size_t i) { assert(!exists(i)); }

  const T& index(size_t i) const { return data()[i]; }
  T& index(size_t i) { return data()[i]; }
public:
  CheckedLazyArray() { memset(allocd, 0, sizeof(allocd)); }

  bool in_bounds(size_t i) const { return i < N; }

  const T* data() const { return _data.data(); }
  T* data() { return (T*) _data.data(); }

  const T& operator[](size_t i) const { return index(i); }
  T& operator[](size_t i) { return index(i); }
  bool exists(size_t i) const {
    return in_bounds(i) && allocd[i >> 3] & (1 << (i & 3)) != 0;
  }

  const T& at(size_t i) const { check_exists(i); return index(i); }
  T& at(size_t i) { check_exists(i); return index(i); }

  T& construct_element(size_t i) {
    check_not_exists(i);
    set_allocd(i, true);
    return _data.construct_element(i);
  }
  template<typename... args>
  T& construct_element(size_t i, args... a) {
    check_not_exists(i);
    set_allocd(i, true);
    return _data.construct_element(i, a...);
  }

  void destruct_element(size_t i) {
    check_exists(i);
    _data.destruct_element(i);
    set_allocd(i, false);
  }
};

class Dummy {
  int my_x;
public:
  int x() const { return my_x; }
  Dummy(int x) : my_x(x) {}
  ~Dummy() { my_x = -1; }
};

int main() {
  auto v = CheckedLazyArray<100, Dummy>();

  const auto& d = v.construct_element(4, 1337);
  printf("Newly Allocated(%u): %d\n", v.exists(4), d.x());
  v.destruct_element(4);
  printf("After Deletion(%u): %d\n", v.exists(4), d.x());

  printf("size: %lu", sizeof(v));
  return 0;
}
