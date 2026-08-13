#include "braceview.hpp"
#include "baxters.hpp"
#include "combview.hpp"
#include "dice.hpp"
#include "idomain.hpp"
#include "opts.hpp"
#include "permutations.hpp"
#include "permview.hpp"
#include "tabtree.hpp"
#include "twintree.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <set>
#include <stdexcept>
#include <vector>

namespace {

void require(bool Condition, const char *Message) {
  if (!Condition)
    throw std::runtime_error(Message);
}

template <typename Exception, typename Function>
void require_throws(Function &&Operation, const char *Message) {
  try {
    Operation();
  } catch (const Exception &) {
    return;
  }
  throw std::runtime_error(Message);
}

void test_permutation_view() {
  using Dom = permutations::IDomain<int>;
  Dom::init(4);

  std::set<std::vector<int>> Seen;
  for (auto Permutation : permutations::permutations_view<Dom>{}) {
    std::vector<int> Plain;
    for (auto Element : Permutation)
      Plain.push_back(Element);
    Seen.insert(std::move(Plain));
  }

  require(Seen.size() == 24, "permutations_view must produce 4! unique values");
}

void test_inverse_round_trip() {
  using Dom = permutations::IDomain<int>;
  Dom::init(4);
  std::vector<Dom> Plain = {2, 4, 1, 3};
  std::vector<permutations::PermLoop<Dom>> Loops;
  permutations::create_loops(Plain.begin(), Plain.end(), std::back_inserter(Loops));

  permutations::Permutation<Dom> Original(Loops.begin(), Loops.end());
  auto RoundTrip = permutations::invert(permutations::invert(Original));
  require(RoundTrip == Original, "inverting twice must recover the permutation");
  require(Original.present_as_perm() == Plain, "loop and plain forms must agree");

  Dom::init(2, -1);
  std::vector<Dom> ShiftedPlain = {0, -1, 2, 1};
  std::vector<permutations::PermLoop<Dom>> ShiftedLoops;
  permutations::create_loops(ShiftedPlain.begin(), ShiftedPlain.end(),
                             std::back_inserter(ShiftedLoops));
  permutations::Permutation<Dom> Shifted(ShiftedLoops.begin(), ShiftedLoops.end());
  require(Shifted.present_as_perm() == ShiftedPlain,
          "plain form must respect a non-one domain minimum");
  std::vector<int> TooShort = {-1, 0, 1};
  require_throws<std::runtime_error>(
      [&] {
        permutations::checked_permutation<Dom>(TooShort.begin(), TooShort.end());
      },
      "checked permutation construction must require the complete domain");
  std::vector<int> Outside = {-1, 0, 1, 3};
  require_throws<std::runtime_error>(
      [&] { permutations::checked_permutation<Dom>(Outside.begin(), Outside.end()); },
      "checked permutation construction must enforce domain bounds");
  std::vector<int> Duplicate = {-1, 0, 0, 2};
  require_throws<std::runtime_error>(
      [&] { permutations::checked_permutation<Dom>(Duplicate.begin(), Duplicate.end()); },
      "checked permutation construction must enforce uniqueness");
}

void test_braced_view() {
  std::set<std::string> Seen;
  for (auto Braces : trees::braced_view{4})
    Seen.emplace(Braces.begin(), Braces.end());
  require(Seen.size() == 14, "braced_view(4) must produce Catalan(4) values");

  bool Threw = false;
  try {
    [[maybe_unused]] trees::braced_view Invalid{0};
    Invalid.begin();
  } catch (const std::logic_error &) {
    Threw = true;
  }
  require(Threw, "braced_view(0) must reject an unsupported size");
}

void test_combination_view() {
  std::set<std::vector<int>> Seen;
  for (auto Combination : combs::comb_view{5, 2})
    Seen.emplace(Combination.begin(), Combination.end());
  require(Seen.size() == 10, "comb_view(5, 2) must produce C(5, 2) values");

  Seen.clear();
  for (auto Combination : combs::comb_view{5, 1})
    Seen.emplace(Combination.begin(), Combination.end());
  require(Seen.size() == 5, "comb_view(5, 1) must produce every singleton");

  Seen.clear();
  for (auto Combination : combs::comb_view{5, 0})
    Seen.emplace(Combination.begin(), Combination.end());
  require(Seen == std::set<std::vector<int>>{{}},
          "comb_view(N, 0) must produce the empty combination once");

  Seen.clear();
  for (auto Combination : combs::comb_view{3, 3})
    Seen.emplace(Combination.begin(), Combination.end());
  require(Seen == std::set<std::vector<int>>{{0, 1, 2}},
          "comb_view(N, N) must produce the full combination once");
  require_throws<std::invalid_argument>([] { combs::comb_view{2, 3}.begin(); },
                                        "comb_view must reject K > N");
  require_throws<std::invalid_argument>([] { combs::comb_view{2, -1}.begin(); },
                                        "comb_view must reject a negative K");
}

void test_dice() {
  require_throws<std::runtime_error>([] { rands::Dice::generate(1, 6); },
                                     "Dice must reject generation before initialization");
  std::vector<int> Values = {1, 2, 3};
  require_throws<std::runtime_error>(
      [&] { rands::Dice::rand_shuffle(Values.begin(), Values.end()); },
      "Dice must reject shuffling before initialization");

  rands::Dice::init(42);
  auto First = rands::Dice::generate(0, 100);
  rands::Dice::init(42);
  require(rands::Dice::generate(0, 100) == First,
          "equal Dice seeds must reproduce a sequence");
  rands::Dice::rand_initialize(Values.begin(), Values.end(), -2, 2);
  require(std::all_of(Values.begin(), Values.end(),
                      [](int Value) { return Value >= -2 && Value <= 2; }),
          "Dice initialization must respect its bounds");
}

void test_options() {
  options::Parser Parser;
  Parser.add<int>("count", 3, "sample count");
  Parser.add<bool>("verbose", false);
  require_throws<std::runtime_error>([&] { Parser.get<int>("count"); },
                                     "options cannot be read before parsing");

  char Program[] = "properties";
  char Count[] = "--count=7";
  char Verbose[] = "-verbose";
  char *Arguments[] = {Program, Count, Verbose};
  Parser.parse(3, Arguments);
  require(Parser.parsed() && Parser.get<int>("count") == 7 &&
              Parser.exists("verbose"),
          "option values and flags must be parsed");
  require_throws<std::runtime_error>([&] { Parser.get<int>("missing"); },
                                     "unknown option queries must fail");

  options::Parser Invalid;
  Invalid.add<int>("known", 1);
  char Unknown[] = "--unknown";
  char *InvalidArguments[] = {Program, Unknown};
  std::ostringstream CapturedOut, CapturedErr;
  auto *OldOut = std::cout.rdbuf(CapturedOut.rdbuf());
  auto *OldErr = std::cerr.rdbuf(CapturedErr.rdbuf());
  require_throws<std::runtime_error>([&] { Invalid.parse(2, InvalidArguments); },
                                     "unknown command-line options must fail");
  std::cout.rdbuf(OldOut);
  std::cerr.rdbuf(OldErr);
  require(CapturedOut.str().find("Available options:") != std::string::npos &&
              CapturedErr.str().find("illegal option") != std::string::npos,
          "an unknown option must produce diagnostics");

  options::Parser BadValue;
  BadValue.add<int>("count", 1);
  char CountText[] = "--count=seven";
  char *BadValueArguments[] = {Program, CountText};
  BadValue.parse(2, BadValueArguments);
  require_throws<std::runtime_error>([&] { BadValue.get<int>("count"); },
                                     "ill-typed option values must fail");

  options::Parser EmptyName;
  EmptyName.add<int>("known", 1);
  char Dashes[] = "--";
  char *EmptyNameArguments[] = {Program, Dashes};
  require_throws<std::runtime_error>(
      [&] { EmptyName.parse(2, EmptyNameArguments); },
      "an option made only of dashes must fail");
}

void test_baxter_boundaries() {
  using Dom = permutations::IDomain<int>;
  Dom::init(3);
  std::vector<Dom> Short = {2, 3, 1};
  require(permutations::isBaxters(Short.begin(), Short.end()),
          "every permutation shorter than four is Baxter");
}

void test_trees() {
  trees::TabTree<int> Tree(5);
  for (int Value : {3, 1, 5, 2, 4})
    Tree.addSearchOrder(Value);

  std::ostringstream Edges, Dot, Topology;
  Tree.dumpEL(Edges);
  Tree.dumpDot(Dot);
  Tree.dumpTopo(Topology);
  require(Edges.str().find("3 -- 1") != std::string::npos,
          "tree edge dump must contain the root's left edge");
  require(Dot.str().starts_with("graph {"), "tree dot dump must be a graph");
  require(Topology.str().starts_with("5\n"), "tree topology must report its size");

  require_throws<trees::tree_error<int>>([&] { Tree.addSearchOrder(3); },
                                         "duplicate tree keys must fail");

  std::istringstream MissingOrder{"3 1 2"};
  require(!trees::read_bst_ordered<int>(MissingOrder, false),
          "an incomplete permutation must not produce a tree");
  std::istringstream InvalidBraces{"2 (x)"};
  require_throws<std::runtime_error>(
      [&] { trees::read_bst_braced<int>(InvalidBraces, false); },
      "invalid brace input must fail");

  std::vector<int> Permutation = {2, 4, 1, 3};
  auto OriginalPermutation = Permutation;
  trees::TwinTree Twin(Permutation.begin(), Permutation.size());
  require(Permutation == OriginalPermutation,
          "TwinTree construction must not modify its input range");
  std::ostringstream Table;
  Twin.dumpTable(Table);
  auto Baxter = Twin.toBaxters();
  using Dom = permutations::IDomain<int>;
  Dom::init(4);
  std::vector<Dom> BaxterDomain(Baxter.begin(), Baxter.end());
  require(!Table.str().empty() && permutations::isBaxters(BaxterDomain.begin(), BaxterDomain.end()),
          "a twin tree must convert to a Baxter permutation");

  std::istringstream Unbalanced{"2 )(()"};
  require_throws<std::runtime_error>(
      [&] { trees::read_bst_braced<int>(Unbalanced, false); },
      "unbalanced topology must be rejected before reconstruction");
}

} // namespace

int main() try {
  test_permutation_view();
  test_inverse_round_trip();
  test_braced_view();
  test_combination_view();
  test_dice();
  test_options();
  test_baxter_boundaries();
  test_trees();
  return 0;
} catch (const std::exception &Error) {
  std::cerr << Error.what() << '\n';
  return 1;
}
