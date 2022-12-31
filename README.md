# Concrete mathematics

Different computability and combinatorics experiments and visualizations. Pure fun.

## Building and testing repo

```
cmake -G Ninja -B build -DCMAKE_CXX_COMPILER=g++-11 -DCMAKE_BUILD_TYPE=Release .
cmake --build build
env CTEST_OUTPUT_ON_FAILURE=1 cmake --build build --target test
```

### special configuration options

- Include apps with SDL2 visualization: `-DVISUAL=1`

## Contents

- readbst -- application reads permutation from input and outputs a tree
