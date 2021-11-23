#pragma once

#include "typedefs.hpp"


// This is more of a generic list-type helper rather than being string-specific
template<class ListType>
auto& last(ListType&& list, int offset = 0) {
  return list[list.size() + offset - 1];
}


inline bool ends_with(String$& suffix, String$& string) {
  if (string.size() < suffix.size())
    return false;
  
  for (int i = 0; i < suffix.size(); i++)
    if (last(string, -i) != last(suffix, -i))
      return false;
  
  return true;
}


namespace mysterious_string_stuff {

namespace to_string_helper {
  // Based upon code by Adam Nevraumont (stackoverflow.com/a/33399801)
  // I don't understand it but it just works.
  template<class T>
  std::string as_string(T&& t) {
    using std::to_string;
    return to_string(std::forward<T>(t));
  }
}

template<class T>
std::string to_string(T&& t) {
  return to_string_helper::as_string(std::forward<T>(t));
}

}

#define $(x...) (mysterious_string_stuff::to_string(x))


template<class Number>
String to_hex_string(Number num, uint hex_digit_count = (sizeof(Number) << 1)) {
  // Based upon code by Andrey Scherbakov (stackoverflow.com/a/33447587)
  static_assert(std::is_integral_v<Number>, "'Number' must be an integer type");
  
  static const char* DIGITS = "0123456789ABCDEF";
  var result = String(hex_digit_count, '0');
  
  // Repeatedly step four bits to the right...
  for (
      uint i = 0, j = (hex_digit_count - 1) * 4;
      i < hex_digit_count;
      ++i, j -= 4
  ) {
    let these_four_bits = (num >> j) & 0b00001111;
    result[i] = DIGITS[these_four_bits];
  }
  
  return result;
}


inline String escape_for_terminal(String$& s) {
  // This function doesn't escape EVERYTHING that could be special in a terminal
  // It's pretty simple. Good enough though.
  String escaped;
  escaped.reserve(s.size());
  
  for (char c : s) {
    switch (c) {
      case '\t': escaped += "\\t"; break;
      case '\r': escaped += "\\r"; break;
      case '\n': escaped += "\\n"; break;
      case '\'': escaped += "'"; break;
      case '"': escaped += "\\\""; break;
      default:
        if (c < 0x20)  // Control character
          escaped += "\\x" + to_hex_string(c, 2);
        else
          escaped.push_back(c);
    }
  }
  
  return escaped;
}