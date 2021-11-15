#pragma once

#include "../types.hpp"

namespace paqFe {

template<int p = ProbEven>
class PresudoModel {
public:
  static constexpr int n_output = 1;

  void init();

  void predict(uint8_t bit, Prob* pp) {
    *pp = p - 1 + bit * 2;
  };
  
  Context getContext() {
    return 0;
  };
};

}