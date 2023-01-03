//------------------------------------------------------------------------------
//
// Input permutation, output is it baxter or not
// Permutation format: N a1 .. aN
//
//------------------------------------------------------------------------------
//
// This file is licensed after LGPL v3
// Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
//
//------------------------------------------------------------------------------

#include "dice.hpp"
#include "opts.hpp"
#include "permutations.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <random>
#include <ranges>
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
  auto Cfg = parse_cfg(argc, argv);

  bool Baxters = true;
  int N;
  std::cin >> N;
  std::vector<int> Vec(N);

  for (int I = 0; I < N; ++I)
    std::cin >> Vec[I];

  using Dom = permutations::IDomain<int>;
  Dom::init(N);

  // all 1, 2 and 3-perms are Baxters by design
  if (Dom::Max() < 4) {
    std::cout << Baxters << std::endl;
    return 0;
  }

  // search for counter examples
  for (int I = 1; (I < Dom::Max()) && Baxters; ++I) {
    auto IdxI = std::ranges::find(Vec, I);
    auto IdxIp = std::ranges::find(Vec, I + 1);

    assert(IdxI != Vec.end());
    assert(IdxIp != Vec.end());

    bool Small = false;
    bool Big = false;

    // IdxI big small IdxIp
    if (IdxIp > IdxI && IdxIp > std::next(IdxI)) {
      for (auto It = std::next(IdxI); It != IdxIp; ++It) {
        if (*It > *IdxI)
          Big = true;
        if (*It < *IdxI && Big) {
          Baxters = false;
          break;
        }
      }
    }
    // IdxIp small big IdxI
    else if (IdxI > IdxIp && IdxI > std::next(IdxIp)) {
      for (auto It = std::next(IdxIp); It != IdxI; ++It) {
        if (*It < *IdxI)
          Small = true;
        if (*It > *IdxI && Small) {
          Baxters = false;
          break;
        }
      }
    }
  }

  std::cout << Baxters << std::endl;
} catch (const std::runtime_error &E) {
  std::cout << "Runtime error: " << E.what() << "\n";
  return -1;
} catch (...) {
  std::cout << "Unknown error\n";
  return -1;
}