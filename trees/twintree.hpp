//------------------------------------------------------------------------------
//
// Twin tree is really thin wrapper around tabulated tree
// it keeps nothing it is just topology
//
// about why this is required, see
// see https://www-cs-faculty.stanford.edu/~knuth/programs/twintree-to-baxter.w
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
#include <vector>

#include "tabtree.hpp"

namespace trees {

namespace ranges = std::ranges;
namespace views = std::views;

class TwinTree {
  int T0 = -1, T1 = -1;
  std::vector<int> D;
  std::vector<int> L0, L1, R0, R1;

public:
  // ctor from permutation
  explicit TwinTree(std::vector<int> &V)
      : D(V.size(), 0), L0(V.size(), 0), L1(V.size(), 0), R0(V.size(), 0),
        R1(V.size(), 0) {
    int N = V.size();

    TabTree<int> Forward(N);
    for (int I = 0; I < N; ++I)
      Forward.addSearchOrder(V[I]);
    Forward.read_structure(T0, L0.begin(), R0.begin(), D.begin());
    std::reverse(V.begin(), V.end());
    TabTree<int> Backward(N);
    for (int I = 0; I < N; ++I)
      Backward.addSearchOrder(V[I]);
    Backward.read_structure(T1, L1.begin(), R1.begin(), D.begin());
    std::reverse(D.begin(), D.end());
    std::reverse(L1.begin(), L1.end());
    std::reverse(R1.begin(), R1.end());
  }

  void dumpTable(std::ostream &Os) const {
    std::ostream_iterator<int> OsIt(Os, " ");
    Os << T0 << " " << T1 << "\n";
    ranges::copy(D, OsIt);
    std::cout << "\n";
    ranges::copy(L0, OsIt);
    Os << "\n";
    ranges::copy(R0, OsIt);
    Os << "\n";
    ranges::copy(L1, OsIt);
    Os << "\n";
    ranges::copy(R1, OsIt);
    Os << "\n";
  }
};

inline std::optional<TwinTree> read_twin_ordered(int N, std::istream &Is) {
  auto OptVec = trees::read_order<int>(N, Is);
  if (!OptVec)
    return std::nullopt;
  auto &Vec = *OptVec;
  TwinTree Ret(Vec);
  return Ret;
}

} // namespace trees