//------------------------------------------------------------------------------
//
// SDL implementation for drawer interface: library part
//
//------------------------------------------------------------------------------
//
// This file is licensed after LGPL v3
// Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
//
//------------------------------------------------------------------------------

#include "sdlutil.hpp"

namespace DrawUtil {

IViewPort *QueryViewPort(const char *Name, int Width, int Height,
                         std::function<void(ISurface *)> Callback,
                         bool Resizable) {
  static ViewPort *V = new ViewPort(Name, Width, Height, Callback, Resizable);
  return V;
}

} // namespace DrawUtil