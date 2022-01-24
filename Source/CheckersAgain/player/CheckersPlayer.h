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

  inline constant BOARD_TAG = TEXT("BOARD");
  inline constant CAMERA_TF_TAG = TEXT("CAMPOINT");
  inline constant WHITE_TAG = TEXT("WHITE");
  inline constant BLACK_TAG = TEXT("BLACK");

public:
  TF white_camera_transform;
  TF black_camera_transform;

  // It's possible that a player controller plays for both black and white.
  bool am_i_white = true;
  bool am_i_black = true;

  bool is_it_my_turn = true;
  bool is_piece_held = false;
  Vec2i held_piece;
  
  ACheckersPlayer();
  
  void on_turn_started();
  UFUNCTION()
  void on_piece_clicked(AActor* piece, FKey button);
  UFUNCTION()
  void on_board_clicked(AActor* some_board_mesh, FKey button);
  
  void initialize();
  void prepare_camera_transforms();
  void make_pieces_clickable();
  void make_board_clickable();
  void toggle_piece_held(Vec2i piece); // Grab the piece or drop it if it's held
  void grab_piece(Vec2i piece);
  void drop_piece();
  void do_move(const CheckersMove& move);

  // "swooshing" is like moving but animated and fancy, okay?
  // Please don't question my naming skills.
  void swoosh_camera_to(const TF& new_camera_tf);

private:
  class ACheckersState* get_gamestate();
  const List<CheckersMove>* cached_moves = nullptr;
};
