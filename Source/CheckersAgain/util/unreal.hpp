#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"


//// Renaming things...

using Vec = FVector;
using Vec2i = FIntPoint;
using Rot = FRotator;
using Plane = FPlane;
using Ray = FRay;
using Color = FColor;
using actors_in_world = TActorRange<AActor>;


template<class ...C>
fn get_class(const TCHAR* path) {
  ConstructorHelpers::FClassFinder<C...> finder(path);
  assert_(finder.Succeeded());
  return finder.Class;
}

template<class ...C>
fn get_object(const TCHAR* path) {
  ConstructorHelpers::FObjectFinder<C...> finder(path);
  assert_(finder.Succeeded());
  return finder.Object;
}

template<class ...C>
fn try_to_get_object(const TCHAR* path) {
  return ConstructorHelpers::FObjectFinderOptional<C...>(path).Object;
}

// Short-hand for the dot product.
template<class C>
fn dot(C a, C b) {
  return C::DotProduct(a, b);
}


//// String stuff

inline String to_string(const FString& unreal_string) {
  return TCHAR_TO_UTF8(ToCStr(unreal_string));
}

inline FString to_unreal_string(String$& normal_string) {
  return normal_string.c_str();
}

inline void print_to_screen(String$& message, Color color = Color::Blue,
                            float duration = 4) {
  std::cout << message << std::endl;
  GEngine->AddOnScreenDebugMessage(-1, duration, color, message.c_str());
}

// "2" serves to prevent ambiguity for the compiler.
inline void print_to_screen2(const FString& message, Color color = Color::Blue,
                             float duration = 4) {
  std::cout << $(message) << std::endl;
  GEngine->AddOnScreenDebugMessage(-1, duration, color, message);
}


inline String to_string(const Vec& vector) {
  return "Vec("+$(vector.X)+","+$(vector.Y)+","+$(vector.Z)+")";
}

inline String to_string(const Vec2i& vector) {
  return "Vec2i("+$(vector.X)+","+$(vector.Y)+")";
}


//// Debugging

#define PRINT_AND_THROW(...) do {           \
  let x = __VA_ARGS__;                      \
  print_to_screen(__VA_ARGS__, Color::Red); \
  THROW(x);                                 \
} while (false);


//// Custom hash functions

namespace std {
  template<>
  struct hash<Vec2i> {
    size_t operator () (const Vec2i& vec) const noexcept {
      return hash<int>()(vec.X) ^ (hash<int>()(vec.Y) << 1);
    }
  };
}
