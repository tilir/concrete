//------------------------------------------------------------------------------
//
// Drawer interface
//
//------------------------------------------------------------------------------
//
// This file is licensed after LGPL v3
// Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
//
//------------------------------------------------------------------------------

#pragma once

#include <cassert>
#include <functional>

namespace DrawUtil {

enum class pollres { PROCEED, STOP };

// unsigned shall be 32-bit type
static_assert(sizeof(unsigned) == 4);

// build color from parts
enum class Offset { A = 24, R = 16, G = 8, B = 0 };

inline constexpr unsigned component(unsigned N, Offset Off) noexcept {
  unsigned Shift = static_cast<unsigned>(Off);
  return N << Shift;
}

inline constexpr unsigned buildcolor(unsigned char A, unsigned char R,
                                     unsigned char G,
                                     unsigned char B) noexcept {
  return component(A, Offset::A) | component(R, Offset::R) |
         component(G, Offset::G) | component(B, Offset::B);
}

template <Offset Off> constexpr unsigned char cpart(unsigned Color) noexcept {
  return Color >> static_cast<unsigned>(Off);
}

constexpr unsigned char cpart_a(unsigned Color) noexcept {
  return cpart<Offset::A>(Color);
}

constexpr unsigned char cpart_r(unsigned Color) noexcept {
  return cpart<Offset::R>(Color);
}

constexpr unsigned char cpart_g(unsigned Color) noexcept {
  return cpart<Offset::G>(Color);
}

constexpr unsigned char cpart_b(unsigned Color) noexcept {
  return cpart<Offset::B>(Color);
}

struct ISurface {
  virtual void putpixel(int X, int Y, unsigned Color) = 0;
  virtual void putlogpixel(double X, double Y, unsigned Color) = 0;
  virtual void fillwith(unsigned Color) = 0;
  virtual unsigned w() const noexcept = 0;
  virtual unsigned h() const noexcept = 0;
  virtual ~ISurface() {}
};

// argument of key binding in special cases
enum KeySpecial { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT };

struct KeyPressed {
  bool is_special;
  union {
    KeySpecial k;
    char s;
  } u;
  KeyPressed(KeySpecial K) : is_special{true}, u{.k = K} {}
  KeyPressed(char S) : is_special{false}, u{.s = S} {}
  char special() const noexcept {
    assert(is_special);
    return u.k;
  }
  char regular() const noexcept {
    assert(!is_special);
    return u.s;
  }
};

struct IViewPort {
  virtual pollres poll() = 0;
  virtual void dump(const char *Name) const = 0;
  virtual void bindkeys(std::function<void(KeyPressed)> KeyCallback) = 0;
  virtual ~IViewPort() {}
};

IViewPort *QueryViewPort(const char *Name, int Width, int Height,
                         std::function<void(ISurface *)> Callback,
                         bool Resize = false);

} // namespace DrawUtil
