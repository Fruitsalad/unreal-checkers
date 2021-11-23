#pragma once


#include "no_deduce.hpp"


/** Returns `number` if `min <= eat_number <= max`, otherwise returns `min` if
 * `number < min` or `max` if `eat_number > max` */
template<class Scalar>
Scalar clamp(Scalar number, no_deduce_<Scalar> min, no_deduce_<Scalar> max) {
  // Clarifying on the use of no_deduce_: Only the number argument should be
  // used by the compiler to try to automatically derive the type of Scalar.
  // no_deduce_ ensures that the type of the min and the max argument aren't
  // a factor in the type deduction process.
  return std::min(std::max(number, min), max);
}



/** `lerp` stands for "linear interpolation" (I didn't come up with the
 * abbreviation)
 *
 * This returns a value between `start` and `end`, with the value being closer
 * to `end` if `interpolationFactor` is closer to 1 and the value being closer
 * to `start` if `interpolationFactor` is closer to 0. */
template<class T>
T lerp(T start, no_deduce_<T> end, double interpolationFactor) {
  return (1-interpolationFactor) * start + interpolationFactor * end;
}


/** Unlike the operator "%", this always returns a positive number. */
template<class Scalar>
Scalar mod(Scalar a, no_deduce_<Scalar> modulo) {
  if (a >= 0)
    return a % modulo;
  else return (a % modulo) + modulo;
}


/** A helper function purely for aesthetics. */
template<class C, class D>
bool was_found(const C& iterator, const D& container) {
  return iterator != container.end();
}