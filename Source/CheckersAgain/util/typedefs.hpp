#pragma once

#include <limits>
#include <type_traits>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <string>

using uint = unsigned int;
using byte = unsigned char;
using String = std::string;
using String$ = const String;  // Fun fact: '$' is a valid identifier char

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;

using std::make_unique;
using std::move;

template<class ...C> using Unique = std::unique_ptr<C...>;
template<class ...C> using Shared = std::shared_ptr<C...>;
template<class ...C> using Weak = std::weak_ptr<C...>;
template<class ...C> using List = std::vector<C...>;
template<class ...C> using Array = std::array<C...>;
template<class ...C> using Map = std::unordered_map<C...>;
template<class ...C> using Function = std::function<C...>;
template<class ...C> using Limits = std::numeric_limits<C...>;
template<class ...C> using Pair = std::pair<C...>;

template<class ...C> using U = std::unique_ptr<C...>;

template<class C> constexpr C max_of = Limits<C>::max();
template<class C> constexpr C min_of = Limits<C>::lowest();
#define max_of(...) (max_of<TypeOf(__VA_ARGS__)>)
#define min_of(...) (min_of<TypeOf(__VA_ARGS__)>)
#define length_of(...) (sizeof(__VA_ARGS__) / sizeof((__VA_ARGS__)[0]))
// $(...) is defined in string.hpp

#define TypeOf(...) decltype(__VA_ARGS__)
#define TypeOfRef(...) std::decay_t<TypeOf(__VA_ARGS__)>
#define TypeOfPtr(...) TypeOfRef(*(__VA_ARGS__))

// Slightly risky auto-abuse. Just #undef this when it's causing trouble.
#define let const auto
#define var auto
// "fn" is short for "function". I mostly use it for lambda functions but it
// also works on normal functions.
#define fn auto
