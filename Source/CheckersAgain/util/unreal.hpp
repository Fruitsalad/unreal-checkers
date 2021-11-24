#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"

using Vec = FVector;
using Vec2i = FIntVector2;
using Plane = FPlane;
using Ray = FRay;

template<class ...C>
auto get_class(const TCHAR* path) {
  ConstructorHelpers::FClassFinder<C...> finder(path);
  assert_(finder.Succeeded());
  return finder.Class;
}

template<class ...C>
auto get_object(const TCHAR* path) {
  ConstructorHelpers::FObjectFinder<C...> finder(path);
  assert_(finder.Succeeded());
  return finder.Object;
}

template<class ...C>
auto try_to_get_object(const TCHAR* path) {
  return ConstructorHelpers::FObjectFinderOptional<C...>(path).Object;
}

// Short-hand for the dot product.
template<class C>
fn dot(C a, C b) {
  return C::DotProduct(a, b);
}
