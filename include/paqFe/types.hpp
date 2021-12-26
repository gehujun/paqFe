#pragma once

#include <cstddef>
#include <cstdint>

namespace paqFe {

enum OpMode {
  Compress,
  Decompress
};

namespace internal {

typedef int32_t Weight;
typedef uint32_t Prob;
typedef uint8_t Context;

constexpr Prob ProbEven = 2048;
constexpr Prob ProbFirst = 2048;

template<typename T>
constexpr bool isPow2(const T& t) {
  return (t > 0 && (((t & (t - 1)) == 0)));
}

static constexpr int max(int a, int b) {
  if(a > b)
    return a;
  else
    return b;
}
/*
template<int z, int x>
class FixedPoint {
  uint32_t x;
  FixedPoint(uint32_t x) : x(x) {};

  template<int z1, int x1>
  FixedPoint<max(z, x1), max(z, x2)> operatro+(const FixedPoint<z1,x1>& other) {
    return FixedPoint<std::max(z, x1), std::max(z, x2)> (x + other);
  }
};
*/

}
}