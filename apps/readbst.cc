//------------------------------------------------------------------------------
//
// Read permutation from stdin and create bst
// Permutation format: N a1 .. aN
// Dump resulting bst to stdout (plenty of options)
//
// try:
// > ./randperm --first=1 --last=10 >& perm
// > ./readbst < perm
//
//------------------------------------------------------------------------------
//
// This file is licensed after LGPL v3
// Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
//
//------------------------------------------------------------------------------

#include "opts.hpp"
#include "tabtree.hpp"
#include <iostream>

namespace {

constexpr bool DEF_VERBOSE = false;
constexpr bool DEF_DOT = false;
constexpr bool DEF_BACK = false;

struct Config {
  bool Verbose = DEF_VERBOSE;
  bool DumpDot = DEF_DOT;
  bool Back = DEF_BACK;
};

Config parse_cfg(int argc, char **argv) {
  Config Cfg;
  options::Parser OptParser;
  OptParser.template add<int>("dot", DEF_DOT, "output dot file");
  OptParser.template add<int>("back", DEF_BACK, "read input backwards");
  OptParser.template add<int>("verbose", DEF_VERBOSE, "a lot of debug output");
  OptParser.parse(argc, argv);

  Cfg.DumpDot = OptParser.exists("dot");
  Cfg.Back = OptParser.exists("back");
  Cfg.Verbose = OptParser.exists("verbose");
  return Cfg;
}

template <typename T>
trees::TabTree<T> read_bst_ordered(std::istream &Is, const Config &Cfg) {
  int N;
  Is >> N;
  std::vector<T> Vec(N);
  for (int I = 0; I < N; ++I)
    Is >> Vec[I];

  if (Cfg.Back)
    std::reverse(Vec.begin(), Vec.end());

  trees::TabTree<T> Ret(N);
  for (int I = 0; I < N; ++I)
    Ret.addSearchOrder(Vec[I]);
  return Ret;
}

} // namespace

int main(int argc, char **argv) try {
  auto Cfg = parse_cfg(argc, argv);
  std::cin.exceptions(std::ios_base::failbit);
  auto Tree = read_bst_ordered<int>(std::cin, Cfg);
  if (Cfg.DumpDot)
    Tree.dumpDot(std::cout);
  else
    Tree.dumpEL(std::cout);
} catch (const trees::tree_error_base &T) {
  std::cout << "Tree error: " << T.what() << " at key: ";
  T.dump_key(std::cout);
  std::cout << "\n";
  return -1;
} catch (const std::runtime_error &E) {
  std::cout << "Runtime error: " << E.what() << "\n";
  return -1;
} catch (...) {
  std::cout << "Unknown error\n";
  return -1;
}