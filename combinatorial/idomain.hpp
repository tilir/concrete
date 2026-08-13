//------------------------------------------------------------------------------
//
// Integral domain
//
//------------------------------------------------------------------------------
//
// This file is licensed after LGPL v3
// Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
//
//------------------------------------------------------------------------------

#pragma once

#include <cassert>
#include <concepts>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace permutations {

//------------------------------------------------------------------------------
//
// Integral domain -- simplect version, nonfixed bound (need to call init)
//
//------------------------------------------------------------------------------

template <typename T> concept Domain = requires(T D) {
  T::Min();
  T::Max();
  typename T::type;
};

template <std::integral T> class IDomain {
  static T Maximum;
  static T Minimum;
  T Val;

public:
  using type = T;

  static void init(T Max, T Min = 1) {
    assert(Max >= Min);
    Maximum = Max;
    Minimum = Min;
  }
  static T Max() { return Maximum; }
  static T Min() { return Minimum; }

  IDomain() : Val(Minimum) {}
  IDomain(T V) : Val(V) { assert(V >= Minimum && V <= Maximum); }
  constexpr operator T() const { return Val; }

  // do not define this: ambiguity vs builtin operator can happen
  // constexpr auto operator<=>(const IDomain &) const = default;
};

// Checked boundary between external values and algorithms whose precondition is
// a complete permutation of the current domain.
template <Domain D, std::forward_iterator It>
std::vector<D> checked_permutation(It B, It E) {
  using T = typename D::type;
  const auto DomainSize = D::Max() - D::Min() + 1;
  if (std::distance(B, E) != DomainSize)
    throw std::runtime_error("Permutation size does not match its domain");

  std::vector<bool> Seen(static_cast<std::size_t>(DomainSize), false);
  std::vector<D> Result;
  Result.reserve(static_cast<std::size_t>(DomainSize));
  for (; B != E; ++B) {
    const auto Value = static_cast<T>(*B);
    if (Value < D::Min() || Value > D::Max())
      throw std::runtime_error("Permutation element is outside its domain");
    const auto Index = static_cast<std::size_t>(Value - D::Min());
    if (Seen[Index])
      throw std::runtime_error("Permutation elements shall be unique");
    Seen[Index] = true;
    Result.emplace_back(Value);
  }
  return Result;
}

template <std::integral T> inline T IDomain<T>::Maximum = 1;
template <std::integral T> inline T IDomain<T>::Minimum = 1;

template <std::integral T>
inline std::ostream &operator<<(std::ostream &Os, const IDomain<T> &Elt) {
  T Underlying = Elt;
  Os << Underlying;
  return Os;
}

template <std::integral T>
inline std::istream &operator>>(std::istream &Is, IDomain<T> &Elt) {
  T Underlying;
  Is >> Underlying;
  if (Is)
    Elt = Underlying;
  return Is;
}

} // namespace permutations

//------------------------------------------------------------------------------
//
// Hash function to behave like underlying for unordered containers
//
//------------------------------------------------------------------------------

namespace std {
template <integral T> struct hash<permutations::IDomain<T>> {
  size_t operator()(const permutations::IDomain<T> &K) const {
    hash<T> H;
    return H(K);
  }
};
} // namespace std
