#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "../blueprintbase/PieceBase.h"
#include "../util.hpp"
#include "CheckersAgain/map/BoardVisualizer.h"
#include "CheckersState.generated.h"

UCLASS()
class CHECKERSAGAIN_API ACheckersState
                        : public AGameStateBase,
                          public I_BoardVisualizer {
  GENERATED_BODY()
  
  UClass* piece_class;
public:
  class ABoard* board;
  Map<Vec2i, APieceBase*> piece_actors;

  bool is_whites_turn = true;
  bool can_player_attack = false;
  bool has_already_moved = false;
  
  ACheckersState();
  void BeginPlay() override;

  void on_piece_killed(Vec2i piece) override;
  void on_piece_promoted(Vec2i piece) override;
  void on_piece_moved(Vec2i piece, Vec2i new_position) override;
  
  void spawn_pieces();

  /** This function automagically guesses what kind of move it is. */
  void commit_move(Vec2i piece, Vec2i destination);
  void end_turn();  // Normally called by commit_move.
  void end_game(bool did_white_win);
};



