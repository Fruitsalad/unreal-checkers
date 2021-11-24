#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "../util.hpp"
#include "BoardVisualizer.generated.h"

UINTERFACE()
class UBoardVisualizer : public UInterface {
  GENERATED_BODY()
};

class CHECKERSAGAIN_API IBoardVisualizer {
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Checkers")
  bool move_piece(int piece_x, int piece_y, int new_pos_x, int new_pos_y);
  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Checkers")
  bool kill_piece(int piece_x, int piece_y);
  UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Checkers")
  bool promote_piece(int piece_x, int piece_y);
};
