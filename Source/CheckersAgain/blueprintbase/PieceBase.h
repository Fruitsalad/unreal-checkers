#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PieceBase.generated.h"

UCLASS()
class CHECKERSAGAIN_API APieceBase : public AActor {
  GENERATED_BODY()
public:
  APieceBase();

  UFUNCTION(BlueprintImplementableEvent)
  void SetKing(bool IsKing);
  UFUNCTION(BlueprintImplementableEvent)
  void SetColor(bool IsWhite);
};
