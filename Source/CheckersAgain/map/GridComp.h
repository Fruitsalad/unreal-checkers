#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../util.hpp"
#include "GridComp.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CHECKERSAGAIN_API UGridComp final : public UActorComponent {
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category=Grid) float tileWorldSize = 100;

	UGridComp();
	Vec calc_cell_center(int x, int y) const;
	Vec calc_cell_center(Vec2i pos) const;
	Vec2i calc_nearest_cell(Vec world_pos) const;
	Plane get_grid_plane() const;
	bool intersects_ray(Ray ray, Vec2i* result_cell = nullptr) const;
};
