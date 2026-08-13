//------------------------------------------------------------------------------
//
// Compile-time debug stream controlled by the ANALYZE definition.
// With ANALYZE=0, expressions streamed through dbgs are not evaluated.
//
//------------------------------------------------------------------------------
//
// This file is licensed after LGPL v3
// Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
//
//------------------------------------------------------------------------------

#pragma once

#include <iostream>

#ifndef ANALYZE
#define ANALYZE 0
#endif

#define dbgs                                                                   \
  if (!ANALYZE) {                                                              \
  } else                                                                       \
    std::cout
