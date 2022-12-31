//------------------------------------------------------------------------------
//
// Tabulated tree: binary tree with left and right vectors
// optimized for almost immutable trees
//
//------------------------------------------------------------------------------
//
// This file is licensed after LGPL v3
// Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
//
//------------------------------------------------------------------------------

#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

namespace trees {

template <typename T> class TabTree {
  std::vector<int> Left, Right;
  std::vector<T> Data;
  int Root;
  int FreePos = 0;

  // Pos is right or left link in parent
  template <typename It> void addNode(It Pos, T Dt) {
    assert(FreePos < Data.size());
    *Pos = FreePos;
    Data[FreePos] = std::move(Dt);
    FreePos += 1;
  }

  void addSearchOrderRec(int N, T Dt) {
    if (Dt < Data[N]) {
      if (Left[N] != -1)
        addSearchOrderRec(Left[N], std::move(Dt));
      else
        addNode(Left.begin() + N, std::move(Dt));
    } else {
      assert(Dt != Data[N]);
      if (Right[N] != -1)
        addSearchOrderRec(Right[N], std::move(Dt));
      else
        addNode(Right.begin() + N, std::move(Dt));
    }
  }

  template <typename It> void setRanks(It Ranks, int N) const {
    if (Left[N] != -1) {
      Ranks[Left[N]] = Ranks[N] + 1;
      setRanks(Ranks, Left[N]);
    }
    if (Right[N] != -1) {
      Ranks[Right[N]] = Ranks[N] + 1;
      setRanks(Ranks, Right[N]);
    }
  }

  // like dumpEL but with nil node and dot specifics
  // mark left links red, right links blue
  void dumpELDot(std::ostream &os) const {
    for (int I = 0; I < Data.size(); ++I) {
      os << Data[I] << " -- ";
      if (Left[I] != -1)
        os << Data[Left[I]] << " [color = red]\n";
      else
        os << "nil [style=dotted]\n";

      os << Data[I] << " -- ";
      if (Right[I] != -1)
        os << Data[Right[I]] << " [color = blue]\n";
      else
        os << "nil [style=dotted]\n";
    }
  }

public:
  explicit TabTree(int Sz, int Rt = -1)
      : Left(Sz, -1), Right(Sz, -1), Data(Sz), Root(Rt) {}

  void addSearchOrder(T Dt) {
    // if Root is -1 this is first node, add to 0 and set Root to 0.
    if (Root == -1) {
      assert(FreePos == 0);
      Data[0] = std::move(Dt);
      Root = 0;
      FreePos += 1;
      return;
    }

    addSearchOrderRec(Root, std::move(Dt));
  }

  // dump tree as edge list
  void dumpEL(std::ostream &os) const {
    for (int I = 0; I < Data.size(); ++I) {
      if (Left[I] != -1)
        os << Data[I] << " -- " << Data[Left[I]] << "\n";
      if (Right[I] != -1)
        os << Data[I] << " -- " << Data[Right[I]] << "\n";
    }
  }

  // dump tree as a dot format to visualize
  // mark left links red, right links blue
  void dumpDot(std::ostream &os) const {
    os << "graph {\n";
    if (Root == -1) {
      os << "}\n";
      return;
    }

    // pretty tree: determine same-rank elements
    assert(Root == 0);
    std::vector<int> Ranks(Data.size(), -1);
    Ranks[Root] = 0;
    os << "{ rank=source " << Data[Root] << "}\n";
    os << "{ rank=sink nil }\n";
    setRanks(Ranks.begin(), 0);
    std::multimap<int, int> MRanks;
    for (int I = 0; I < Data.size(); ++I)
      MRanks.insert({Ranks[I], I});
    auto MaxRankIt = std::max_element(Ranks.begin(), Ranks.end());
    if (MaxRankIt != Ranks.end()) {
      for (int I = 1; I < *MaxRankIt; ++I) {
        auto [It, Ite] = MRanks.equal_range(I);
        if (It != Ite) {
          os << "{ rank=same ";
          for (auto EltIt = It; EltIt != Ite; ++EltIt)
            os << Data[EltIt->second] << " ";
          os << " }\n";
        }
      }
    }

    dumpELDot(os);
    os << "}\n";
  }
};

} // namespace trees