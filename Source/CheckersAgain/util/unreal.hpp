#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"


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
  assert_msg(finder.Succeeded(),
      ("Failed to find class \""+String(TCHAR_TO_UTF8(path))+"\"").c_str());
  return finder.Class;
}

// Helper function to use AActor as the default for get_class
inline fn get_class(const TCHAR* path) {
  return get_class<AActor>(path);
}

template<class ...C>
fn get_object(const TCHAR* path) {
  ConstructorHelpers::FObjectFinder<C...> finder(path);
  assert_(finder.Succeeded(),
      ("Failed to find object \""+String(TCHAR_TO_UTF8(path))+"\"").c_str());
  return finder.Object;
}

inline fn get_mesh(const TCHAR* path) {
  return get_object<UStaticMesh>(path);
}

inline fn get_material(const TCHAR* path) {
  return get_object<UMaterial>(path);
}

inline fn get_material_inst(const TCHAR* path) {
  return get_object<UMaterialInstance>(path);
}

template<class ...C>
fn try_to_get_object(const TCHAR* path) {
  return ConstructorHelpers::FObjectFinderOptional<C...>(path).Object;
}

template<class C>
fn get_actor_of_class(UWorld* world) {
  return Cast<C>(UGameplayStatics::GetActorOfClass(world, C::StaticClass()));
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


//// Static meshes

inline AStaticMeshActor* spawn_static_mesh(
    UWorld* world,
    UStaticMesh* mesh,
    Vec location,
    Rot rotation) {
  var new_actor = world->SpawnActor(AStaticMeshActor::StaticClass(),
                                    &location, &rotation);
  var mesh_actor = Cast<AStaticMeshActor>(new_actor);
  mesh_actor->GetStaticMeshComponent()->SetStaticMesh(mesh);
  return mesh_actor;
}

inline AStaticMeshActor* spawn_static_mesh(
    UWorld* world,
    UStaticMesh* mesh,
    Vec location,
    Rot rotation,
    UMaterialInterface* material) {
  var new_actor = world->SpawnActor(AStaticMeshActor::StaticClass(),
                                    &location, &rotation);
  var mesh_actor = Cast<AStaticMeshActor>(new_actor);
  var mesh_comp = mesh_actor->GetStaticMeshComponent();
  mesh_comp->SetStaticMesh(mesh);
  mesh_comp->SetMaterial(0, material);
  return mesh_actor;
}