//------------------------------------------------------------------------------
//
// Baxter permutations service functions
//
//------------------------------------------------------------------------------
//
// This file is licensed after LGPL v3
// Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
//
//------------------------------------------------------------------------------

#pragma once

#include <algorithm>
#include <iterator>
#include <ranges>

#include "idomain.hpp"

namespace permutations {

namespace ranges = std::ranges;
namespace views = std::views;

template <std::forward_iterator It> bool isBaxters(It VBegin, It VEnd) {
  using Dom = typename std::decay<decltype(*VBegin)>::type;
  static_assert(Domain<Dom>);

  const auto DomainSize = Dom::Max() - Dom::Min() + 1;
  assert(std::distance(VBegin, VEnd) == DomainSize);

  if (DomainSize < 4)
    return true;

  auto VecR = ranges::subrange(VBegin, VEnd);

  // search for counter examples
  for (int I = Dom::Min(); I < Dom::Max(); ++I) {
    Dom DI = I, DIp = DI + 1;
    auto IdxI = ranges::find(VecR, DI);
    auto IdxIp = ranges::find(VecR, DIp);

    assert(IdxI != VEnd);
    assert(IdxIp != VEnd);

    // interesting: we may capture and test IdxIp as well
    auto BigCond = [IdxI](auto V) { return V > *IdxI; };
    auto SmallCond = [IdxI](auto V) { return V < *IdxI; };

    // IdxI big small IdxIp
    if (IdxIp > IdxI) {
      auto SubVec = ranges::subrange(std::next(IdxI), IdxIp);
      auto BigIt = ranges::find_if(SubVec, BigCond);
      SubVec = ranges::subrange(BigIt, IdxIp);
      auto SmallIt = ranges::find_if(SubVec, SmallCond);
      if (SmallIt != IdxIp)
        return false;
    }
    // IdxIp small big IdxI
    else if (IdxI > IdxIp) {
      auto SubVec = ranges::subrange(std::next(IdxIp), IdxI);
      auto SmallIt = ranges::find_if(SubVec, SmallCond);
      SubVec = ranges::subrange(SmallIt, IdxI);
      auto BigIt = ranges::find_if(SubVec, BigCond);
      if (BigIt != IdxI)
        return false;
    }
  }
  return true;
}

} // namespace permutations
