//------------------------------------------------------------------------------
//
// Output random permutation (default is 1 .. N)
// Permutation format: N a1 .. aN
//
// try:
// > ./randperm --last=10
//
//------------------------------------------------------------------------------
//
// This file is licensed after LGPL v3
// Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
//
//------------------------------------------------------------------------------

#include "opts.hpp"
#include "tabtree.hpp"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>

namespace {

constexpr int DEF_FIRST = 1;
constexpr int DEF_LAST = 10;
constexpr bool DEF_VERBOSE = false;

struct Config {
  int First = DEF_FIRST;
  int Last = DEF_LAST;
  bool Verbose = DEF_VERBOSE;
};

Config parse_cfg(int argc, char **argv) {
  Config Cfg;
  options::Parser OptParser;
  OptParser.template add<int>("verbose", DEF_VERBOSE, "a lot of debug output");
  OptParser.template add<int>("first", DEF_FIRST, "starting permutation value");
  OptParser.template add<int>("last", DEF_LAST, "ending permutation value");
  OptParser.parse(argc, argv);

  Cfg.Verbose = OptParser.exists("verbose");
  Cfg.First = OptParser.get<int>("first");
  Cfg.Last = OptParser.get<int>("last");
  return Cfg;
}

} // namespace

int main(int argc, char **argv) try {
  auto Cfg = parse_cfg(argc, argv);
  std::random_device Rd;
  std::mt19937 Rng{Rd()};

  std::vector<int> V(Cfg.Last - Cfg.First + 1);
  std::iota(V.begin(), V.end(), Cfg.First);
  std::shuffle(V.begin(), V.end(), Rng);

  std::cout << V.size() << "\n";
  std::ostream_iterator<int> OsIt(std::cout, " ");
  std::copy(V.begin(), V.end(), OsIt);
  std::cout << "\n";
} catch (const std::runtime_error &E) {
  std::cout << "Runtime error: " << E.what() << "\n";
  return -1;
} catch (...) {
  std::cout << "Unknown error\n";
  return -1;
}