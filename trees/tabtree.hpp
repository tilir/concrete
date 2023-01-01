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

#pragma once

#include <algorithm>
#include <concepts>
#include <iostream>
#include <map>
#include <optional>
#include <ranges>
#include <stack>
#include <stdexcept>
#include <tuple>
#include <vector>

namespace trees {

namespace ranges = std::ranges;

struct tree_error_base : public std::runtime_error {
  virtual void dump_key(std::ostream &) const = 0;
  tree_error_base(const char *Msg) : std::runtime_error(Msg) {}
  const char *what() const noexcept override {
    return std::runtime_error::what();
  }
};

template <typename T> struct tree_error : public tree_error_base {
  T Key;
  tree_error(const char *Msg, T K) : tree_error_base(Msg), Key(K) {}
  void dump_key(std::ostream &Os) const override { Os << Key; }
};

// tabulated tree class
template <typename T> class TabTree {
  std::vector<int> Left, Right;
  std::vector<T> Data;
  int Root = -1;
  int FreePos = 0;

  // Pos is right or left link in parent
  template <std::random_access_iterator It> void addNode(It Pos, T Dt) {
    assert(FreePos < Data.size());
    *Pos = FreePos;
    Data[FreePos] = std::move(Dt);
    FreePos += 1;
  }

  // ineffective, may be in future I will rewrite better
  void addSearchOrderRec(int N, T Dt) {
    if (Dt == Data[N])
      throw tree_error<T>("Error: duplicate key insertion", Dt);
    if (Dt < Data[N]) {
      if (Left[N] != -1)
        addSearchOrderRec(Left[N], std::move(Dt));
      else
        addNode(Left.begin() + N, std::move(Dt));
    } else {
      if (Right[N] != -1)
        addSearchOrderRec(Right[N], std::move(Dt));
      else
        addNode(Right.begin() + N, std::move(Dt));
    }
  }

  // ineffective, may be in future I will rewrite better
  template <std::random_access_iterator It>
  void setRanks(It Ranks, int N) const {
    if (Left[N] != -1) {
      Ranks[Left[N]] = Ranks[N] + 1;
      setRanks(Ranks, Left[N]);
    }
    if (Right[N] != -1) {
      Ranks[Right[N]] = Ranks[N] + 1;
      setRanks(Ranks, Right[N]);
    }
  }

  // like dumpEL but unordered, with nil node and dot specifics
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

  // ineffective, may be in future I will rewrite better
  template <typename F> void visit_inord_rec(int N, F Visitor) {
    if (Left[N] != -1)
      visit_inord_rec(Left[N], Visitor);
    Visitor(Data[N]);
    if (Right[N] != -1)
      visit_inord_rec(Right[N], Visitor);
  }

  template <typename F> void visit_inord(F Visitor) {
    if (Root == -1)
      return;
    assert(Root == 0);
    visit_inord_rec(Root, Visitor);
  }

  // main idea: putting in stack node for right and left link
  template <std::random_access_iterator It>
  void reconstructTopology(It Start, int Sz) {
    std::stack<int> S;
    Root = 0;
    int CurNode = 0;
    assert(Start[0] == true);
    S.push(Root);
    S.push(Root);

    for (int N = 1; N < Sz; ++N) {
      auto Parent = S.top();
      S.pop();
      auto Elt = Start[N];
      if (Elt) {
        CurNode += 1;
        if (Left[Parent] == -1) {
          Left[Parent] = CurNode;
        } else {
          assert(Right[Parent] == -1);
          Right[Parent] = CurNode;
        }
        S.push(CurNode);
        S.push(CurNode);
      }
      assert(!S.empty());
    }

    S.pop();
    assert(S.empty());
  }

public:
  explicit TabTree(int Sz, int Rt = -1)
      : Left(Sz, -1), Right(Sz, -1), Data(Sz), Root(Rt) {}

  // construct TabTree from proper braced sequence like (()())()
  // equivalent permutation will be 1 .. N+1 where N is number of braces
  template <std::random_access_iterator It> requires requires(It I) {
    // clang-format off
    { *I } -> std::convertible_to<bool>;
    // clang-format on
  }
  explicit TabTree(It Start, int Sz)
      : Left(Sz / 2, -1), Right(Sz / 2, -1), Data(Sz / 2) {
    reconstructTopology(Start, Sz);

    T CurVal = 1;
    visit_inord([&CurVal](T &Data) { Data = CurVal++; });
  }

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
  // edges are ordered by first and then by second vertex data
  // prepend with nedges and root vertex data
  void dumpEL(std::ostream &Os) const {
    std::vector<std::pair<T, T>> Els;
    Els.reserve(Data.size() * 2);
    for (int I = 0; I < Data.size(); ++I) {
      if (Left[I] != -1)
        Els.emplace_back(Data[I], Data[Left[I]]);
      if (Right[I] != -1)
        Els.emplace_back(Data[I], Data[Right[I]]);
    }

    if (Els.empty()) {
      Os << "[niltree]\n";
      return;
    }

    ranges::sort(Els, {}, [](auto P) { return P.second; });
    ranges::stable_sort(Els, {}, [](auto P) { return P.first; });

    Os << Els.size() << "\n";
    Os << Data[Root] << "\n";
    for (const auto &El : Els)
      Os << El.first << " -- " << El.second << "\n";
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

  template <std::random_access_iterator It>
  void readTopologyRec(int N, It Arr, int &Cursor) const {
    int Cur = Cursor;
    if (N == -1 && Cur < 2 * Data.size()) {
      Arr[Cur] = 0;
      Cursor += 1;
      return;
    }

    if (Cur < 2 * Data.size()) {
      assert(N != -1);
      Arr[Cur] = 1;
      Cursor += 1;
      readTopologyRec(Left[N], Arr, Cursor);
      readTopologyRec(Right[N], Arr, Cursor);
    }
  }

  std::vector<bool> readTopology() const {
    std::vector<bool> Ret;
    if (Root == -1)
      return Ret;
    assert(Root == 0);
    Ret.resize(Data.size() * 2);
    int Cursor = 0;
    readTopologyRec(Root, Ret.begin(), Cursor);
    return Ret;
  }

  void dumpTopo(std::ostream &os) const {
    std::vector<bool> V = readTopology();
    assert((V.size() % 2) == 0);
    std::cout << V.size() / 2 << std::endl;
    std::ostream_iterator<char> OsIt(std::cout, "");
    std::transform(V.begin(), V.end(), OsIt,
                   [](bool S) { return S ? '(' : ')'; });
    std::cout << std::endl;
  }
};

// parse proper braces and read the tree
template <typename T>
trees::TabTree<T> read_bst_braced(std::istream &Is, bool Back) {
  int N, M = 0;
  Is >> N;
  N = N * 2;
  std::vector<bool> Vec(N + 1); // 2N + 1 to fit mandatory last 0
  while (M < N) {
    char c;
    Is >> c;
    if (std::isspace(c))
      continue;
    switch (c) {
    case '(':
    case ')':
      Vec[M] = (c == '(');
      M += 1;
      break;
    default:
      throw std::runtime_error("Unknown input symbol");
    }
  }

  if (Back)
    std::reverse(Vec.begin(), Vec.end());

  trees::TabTree<T> Ret(Vec.begin(), N);
  return Ret;
}

// parse permutation and read the tree
template <typename T>
trees::TabTree<T> read_bst_ordered(std::istream &Is, bool Back) {
  int N;
  Is >> N;
  std::vector<T> Vec(N);
  for (int I = 0; I < N; ++I)
    Is >> Vec[I];

  if (Back)
    std::reverse(Vec.begin(), Vec.end());

  trees::TabTree<T> Ret(N);
  for (int I = 0; I < N; ++I)
    Ret.addSearchOrder(Vec[I]);
  return Ret;
}

} // namespace trees