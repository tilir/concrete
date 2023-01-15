//------------------------------------------------------------------------------
//
// Simple combinatorial object views
// Just indices, not sophisticated domains etc
// * over all permutations 1..n
//
//------------------------------------------------------------------------------
//
// This file is licensed after LGPL v3
// Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
//
//------------------------------------------------------------------------------

#pragma once

#include <algorithm>
#include <cassert>
#include <concepts>
#include <numeric>
#include <ranges>
#include <vector>

namespace cviews {

namespace ranges = std::ranges;
namespace views = std::views;

struct PermutationsSentinel {};

class PermutationsGenerator {
  std::vector<int> State;
  using STT = std::vector<int>;
  bool Proceed = true;

public:
  PermutationsGenerator(int N = 1) : State(N) {
    std::iota(State.begin(), State.end(), 1);
  }

  template <std::forward_iterator It>
  PermutationsGenerator(It Begin, It End) : State(Begin, End) {}

  auto operator*() { return views::all(State); }

  PermutationsGenerator &operator++() {
    Proceed = std::next_permutation(State.begin(), State.end());
    if (!Proceed)
      std::iota(State.begin(), State.end(), 1);
    return *this;
  }

  PermutationsGenerator operator++(int) {
    auto OldVal = *this;
    ++*this;
    return OldVal;
  }

  bool equals(const PermutationsGenerator &Rhs) const {
    return ranges::equal(State, Rhs.State);
  }

  bool proceed() const { return Proceed; }
};

inline bool operator==(const PermutationsGenerator &Lhs,
                       const PermutationsGenerator &Rhs) {
  return Lhs.equals(Rhs) && Lhs.proceed() == Rhs.proceed();
}

inline bool operator!=(const PermutationsGenerator &Lhs,
                       const PermutationsGenerator &Rhs) {
  return !(Lhs == Rhs);
}

inline bool operator==(const PermutationsGenerator &Lhs,
                       const PermutationsSentinel &Rhs) {
  return (Lhs.proceed() == false);
}

inline bool operator!=(const PermutationsGenerator &Lhs,
                       const PermutationsSentinel &Rhs) {
  return !(Lhs == Rhs);
}

class permutations_view : public ranges::view_interface<permutations_view> {
  int N;

public:
  using iterator = PermutationsGenerator;
  using sentinel = PermutationsSentinel;

  permutations_view(int P) : N(P) {}

  iterator begin() { return PermutationsGenerator(N); }

  sentinel end() { return PermutationsSentinel{}; }
};

} // namespace cviews