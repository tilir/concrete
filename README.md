# Concrete mathematics

Composable command-line tools and C++20 building blocks for combinatorial
experiments.

The repository can enumerate and transform permutations, combinations,
balanced-parenthesis sequences, binary search trees, twin trees, and Baxter
permutations. Applications exchange deliberately small text formats, making it
easy to join them with ordinary shell pipelines.

## See it in action

Generate a reproducible permutation and invert it:

```sh
./build/apps/randperm -last=8 -seed=17 | ./build/apps/invperm
```

Enumerate all permutations of size four and classify the Baxter ones:

```sh
echo 4 | ./build/apps/allperms \
  | ./build/apps/is-baxters -batch -combine
```

Generate a permutation and inspect the binary search tree induced by its
insertion order:

```sh
./build/apps/randperm -last=8 -seed=17 | ./build/apps/readbst
```

Convert the same kind of input to canonical cycle notation:

```sh
./build/apps/randperm -last=8 -seed=17 | ./build/apps/permloops
```

Every application supports `-help`. Both `-option=value` and
`--option=value` are accepted.

## Build

The project requires CMake 3.20+, Ruby for the CLI tests, and a C++20 compiler.
With Ninja and Clang installed:

```sh
cmake -S . -B build -G Ninja \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Recent GCC works as well.

## Applications

### Permutations

- `allperms` enumerates every permutation of `1..N`.
- `randperm` shuffles an inclusive integer interval.
- `invperm` prints the inverse of a permutation.
- `permloops` converts plain notation to canonical cycle notation.
- `is-baxters` recognizes Baxter permutations and can act as a batch filter.

### Combinations and balanced parentheses

- `allsubsets` enumerates all subsets of `0..N-1`, including the empty and
  full subsets.
- `allbraces` enumerates balanced-parenthesis strings with `N` pairs.
- `randbraces` generates one random balanced-parenthesis string, optionally
  in zero/one notation.

### Trees

- `readbst` constructs a binary search tree from an insertion order or a
  balanced-parenthesis topology. It can emit an edge list, Graphviz input, or
  topology notation.
- `readtwin` constructs the forward and backward search trees associated with
  a permutation and can produce the corresponding Baxter permutation.

## Formats and composition

A single permutation is represented by its size followed by its elements:

```text
5
4 5 1 2 3
```

Batch producers write the size once and then a sequence of objects having that
size. Consumers with a `-batch` option read the same representation. This is
what allows `allperms` to feed `is-baxters` directly.

Balanced-parenthesis generators use an analogous format: the pair count comes
first, followed by one or more strings. Tree applications accept either the
shared permutation format or, where applicable, this topology format.

Output is intentionally plain and stable: it works well in shell pipelines,
fixture tests, small scripts, and exploratory data processing.

## C++ library

The reusable implementation is header-only:

- `combinatorial/` contains permutation and cycle operations, combinatorial
  views, Baxter recognition, and tree representations.
- `framework/` contains the small option parser, random engine, and diagnostic
  stream.

The library distinguishes trusted algorithm inputs from untrusted external
data. Algorithms such as `create_loops` and `isBaxters` assume a complete
permutation of the active domain and avoid repeating a validation pass on every
composition. Use `checked_permutation` at an I/O or API boundary to validate
size, bounds, and uniqueness first.

`IDomain<T>` keeps its bounds statically per underlying type. Initialize it
before constructing values, and do not expect two independent `IDomain<int>`
ranges to coexist.

## Development

Run the complete suite with:

```sh
ctest --test-dir build --output-on-failure
```

Tests include exact `.dat`/`.ans` CLI fixtures, property checks, and real
multi-process pipelines covering every composition demonstrated above plus
tree/topology conversions. Optional `.err` and `.status` files describe
expected failure output and exit status.

Additional build switches are available for maintenance work:

- `-DENABLE_SANITIZERS=ON` enables AddressSanitizer and
  UndefinedBehaviorSanitizer on GCC or Clang.
- `-DENABLE_COVERAGE=ON` enables the `coverage-report` and `coverage-html`
  targets for Clang/LLVM.
- `-DENABLE_WARNINGS=OFF` disables the warning set enabled by default.

The project is licensed under LGPL v3; see [LICENSE](LICENSE).
