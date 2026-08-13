//------------------------------------------------------------------------------
//
// Enumerate every permutation of the integral domain 1..N.
// Input is a positive N; output is the batch format:
// N
// a1 .. aN
// b1 .. bN
// ........
//
//------------------------------------------------------------------------------
//
// This file is licensed after LGPL v3
// Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
//
//------------------------------------------------------------------------------

#include "dice.hpp"
#include "opts.hpp"
#include "permview.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
#include <vector>

namespace {

constexpr bool DEF_VERBOSE = false;

struct Config {
  bool Verbose = DEF_VERBOSE;
};

Config parse_cfg(int argc, char **argv) {
  Config Cfg;
  options::Parser OptParser;
  OptParser.template add<int>("verbose", DEF_VERBOSE, "a lot of debug output");
  OptParser.parse(argc, argv);

  Cfg.Verbose = OptParser.exists("verbose");
  return Cfg;
}

} // namespace

int main(int argc, char **argv) try {
  parse_cfg(argc, argv);

  int N = 0;
  if (!(std::cin >> N) || N <= 0)
    throw std::runtime_error("Expected a positive permutation size");

  using Dom = permutations::IDomain<int>;
  Dom::init(N);

  permutations::permutations_view<Dom> V;
  std::cout << N << "\n";
  for (auto P : V) {
    for (auto Elt : P)
      std::cout << Elt << " ";
    std::cout << "\n";
  }
} catch (const std::runtime_error &E) {
  std::cout << "Runtime error: " << E.what() << "\n";
  return -1;
} catch (...) {
  std::cout << "Unknown error\n";
  return -1;
}
