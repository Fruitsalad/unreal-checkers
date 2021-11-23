#pragma once


template<class T>
struct type_identity {
  using type = T;
};


// This type wrapper prevents C++ from using an argument to a
// template function to deduce the function's template arguments.
// Take a look at clamp to see how you should use this.
template<class T> using no_deduce_ = typename type_identity<T>::type;