#pragma once

#include <limits>
#include <type_traits>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

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
template<class ...C> using Map = std::unordered_map<C...>;
template<class ...C> using Function = std::function<C...>;
template<class ...C> using Limits = std::numeric_limits<C...>;

template<class ...C> using U = std::unique_ptr<C...>;

template<class C> constexpr C max_of = Limits<C>::max();
template<class C> constexpr C min_of = Limits<C>::lowest();
#define max_of(x...) (max_of<TypeOf(x)>)
#define min_of(x...) (min_of<TypeOf(x)>)
#define length_of(x...) (sizeof(x) / sizeof(x[0]))
// $(x...) is defined in string.hpp

#define TypeOf(x...) decltype(x)
#define TypeOfRef(x...) std::decay_t<TypeOf(x)>
#define TypeOfPtr(x...) TypeOfRef(*(x))

// Slightly risky auto-abuse. Just #undef this when it's causing trouble.
#define let const auto
#define var auto
// "fn" is short for "function". I mostly use it for lambda functions but it
// also works on normal functions.
#define fn auto
