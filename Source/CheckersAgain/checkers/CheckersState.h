#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "../blueprintbase/PieceBase.h"
#include "../util.hpp"
#include "CheckersAgain/map/Board.h"
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
  List<AActor*> board_actors;

  bool is_whites_turn = true;
  bool has_already_moved = false;
  Vec2i piece_being_moved;
  uint longest_available_attack_chain = 0;
  List<CheckersMove> available_moves;
  CheckersRules rules;
  
  ACheckersState();
  void BeginPlay() override;

  void on_piece_killed(Vec2i pos) override;
  void on_piece_promoted(Vec2i pos) override;
  void on_piece_moved(Vec2i pos, Vec2i new_position) override;
  
  void spawn_pieces();
  void spawn_board();

  /** This function automagically guesses what kind of move it is. */
  void move(Vec2i origin, Vec2i destination);
  void maybe_promote(Vec2i pos);
  void end_turn();
  void start_turn();  // Automatically called by end_turn
  void end_game(bool did_white_win);  // Automatically called by start_turn
};



