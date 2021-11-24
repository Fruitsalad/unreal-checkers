#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../util.hpp"
#include "CheckersAgain/blueprintbase/PieceBase.h"
#include "CheckersPlayer.generated.h"

UCLASS()
class CHECKERSAGAIN_API ACheckersPlayer : public APlayerController
{
  GENERATED_BODY()

  UClass* piece_class;
public:
  class ABoard* board;
  List<APieceBase*> piece_actors;
  
  ACheckersPlayer();

  virtual void BeginPlay() override;
  void spawn_pieces();

  // on click tile (or checker piece)
	//
	
};
