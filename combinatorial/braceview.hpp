//------------------------------------------------------------------------------
//
// C++20 view enumerating balanced-parenthesis strings with a fixed pair count.
// Based on TAOCP 7.2.1.6, Algorithm P. A pair count must be positive.
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
#include <iostream>
#include <limits>
#include <numeric>
#include <ranges>
#include <stdexcept>
#include <string>
#include <vector>

#include "dbgs.hpp"

namespace trees {

namespace ranges = std::ranges;
namespace views = std::views;

struct BracedSentinel {};

inline std::size_t braced_storage_size(int N) {
  if (N <= 0 || N > std::numeric_limits<int>::max() / 2)
    throw std::invalid_argument("Brace-pair count shall be positive");
  return static_cast<std::size_t>(N) * 2;
}

class BracedGenerator {
  int N;
  int LeftMostClosing, ActiveTail;
  std::string Braces; // we can also go ahead with vector<char>
  bool Proceed;

public:
  explicit BracedGenerator(int NBrac, bool P = true)
      : N(NBrac), LeftMostClosing(2 * (N - 1)), ActiveTail(LeftMostClosing - 1),
        Braces(braced_storage_size(N), '('), Proceed(P) {
    for (int i = 0; i < N; ++i)
      Braces[2 * i + 1] = ')';
  }

  void restart(bool P) { *this = BracedGenerator(N, P); }

  auto operator*() { return views::all(Braces); }

  BracedGenerator &operator++() {
    // P3: simple case (p == 0)
    dbgs << "\tP3, m = " << LeftMostClosing << std::endl;
    Braces[LeftMostClosing] = ')';
    if (LeftMostClosing > 0 && Braces[LeftMostClosing - 1] == ')') {
      LeftMostClosing -= 1;
      Braces[LeftMostClosing] = '(';
      return *this;
    }

    // P4: look up new active tail
    dbgs << "\tP4 start:" << Braces << std::endl;
    ActiveTail = LeftMostClosing - 1;
    int K = 2 * (N - 1);
    while (ActiveTail >= 0 && Braces[ActiveTail] == '(') {
      Braces[ActiveTail] = ')';
      Braces[K] = '(';
      ActiveTail = ActiveTail - 1;
      K = K - 2;
    }

    // P5: increment active tail position
    dbgs << "\tP5 start:" << Braces << std::endl;
    if (ActiveTail > 0)
      Braces[ActiveTail] = '(';
    LeftMostClosing = 2 * (N - 1);

    Proceed = (ActiveTail != -1);
    if (!Proceed)
      restart(false);

    return *this;
  }

  BracedGenerator operator++(int) {
    auto OldVal = *this;
    ++*this;
    return OldVal;
  }

  bool equals(const BracedGenerator &Rhs) const { return Braces == Rhs.Braces; }

  bool proceed() const { return Proceed; }
};

inline bool operator==(const BracedGenerator &Lhs, const BracedGenerator &Rhs) {
  return Lhs.equals(Rhs) && Lhs.proceed() == Rhs.proceed();
}

inline bool operator!=(const BracedGenerator &Lhs, const BracedGenerator &Rhs) {
  return !(Lhs == Rhs);
}

inline bool operator==(const BracedGenerator &Lhs, const BracedSentinel &) {
  return (Lhs.proceed() == false);
}

inline bool operator!=(const BracedGenerator &Lhs, const BracedSentinel &Rhs) {
  return !(Lhs == Rhs);
}

inline bool operator==(const BracedSentinel &Lhs, const BracedGenerator &Rhs) {
  return Rhs == Lhs;
}

class braced_view : public ranges::view_interface<braced_view> {
  int N;

public:
  using iterator = BracedGenerator;
  using sentinel = BracedSentinel;
  using difference_type = std::ptrdiff_t;

  braced_view(int NBrac) : N(NBrac) {}
  iterator begin() { return iterator{N}; }
  sentinel end() { return sentinel{}; }
};

} // namespace trees
