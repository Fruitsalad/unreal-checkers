#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../util.hpp"
#include "CheckersAgain/map/Board.h"
#include "CheckersPlayer.generated.h"

UCLASS()
class CHECKERSAGAIN_API ACheckersPlayer : public APlayerController
{
  GENERATED_BODY()

public:
  bool is_it_my_turn = true;
  bool is_piece_held = false;
  Vec2i held_piece;
  
  ACheckersPlayer();
  
  void on_turn_started();
  UFUNCTION()
  void on_piece_clicked(AActor* piece, FKey button);
  UFUNCTION()
  void on_board_clicked(AActor* some_board_mesh, FKey button);
  
  virtual void BeginPlay() override;
  
  void bind_inputs();
  void make_pieces_clickable();
  void make_board_clickable();
  void toggle_piece_held(Vec2i piece); // Grab the piece or drop it if it's held
  void grab_piece(Vec2i piece);
  void drop_piece();
  void do_move(const CheckersMove& move);

private:
  class ACheckersState* get_gamestate();
  const List<CheckersMove>* cached_moves = nullptr;
};
