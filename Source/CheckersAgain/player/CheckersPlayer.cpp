#include "CheckersPlayer.h"

#include <cassert>

#include "CheckersAgain/checkers/CheckersState.h"
#include "CheckersAgain/map/Board.h"
#include "CheckersAgain/map/GridComp.h"


using This = ACheckersPlayer;

  
ACheckersPlayer::ACheckersPlayer() {
  SetShowMouseCursor(true);
  bEnableMouseOverEvents = true;
  bEnableClickEvents = true;
}

void ACheckersPlayer::initialize() {
  prepare_camera_transforms();
  make_board_clickable();
  make_pieces_clickable();
}

void ACheckersPlayer::prepare_camera_transforms() {
  // This could be merged with make_board_clickable, but I can't be bothered.
  for (let actor : actors_in_world(GetWorld())) {
    if (actor->ActorHasTag(CAMERA_TF_TAG)) {
      if (actor->ActorHasTag(WHITE_TAG))
        white_camera_transform = actor->GetActorTransform();
      if (actor->ActorHasTag(BLACK_TAG))
        black_camera_transform = actor->GetActorTransform();
    }
  }
}

void ACheckersPlayer::make_pieces_clickable() {
  for (var [position, piece_actor] : get_gamestate()->piece_actors)
    piece_actor->OnClicked.AddDynamic(this, &This::on_piece_clicked);
}

void ACheckersPlayer::make_board_clickable() {
  for (var actor : actors_in_world(GetWorld()))
    if (actor->ActorHasTag(BOARD_TAG))
      actor->OnClicked.AddDynamic(this, &This::on_board_clicked);
}


void ACheckersPlayer::on_turn_started() {
  drop_piece();
  let state = get_gamestate();
  cached_moves = &state->available_moves;

  is_it_my_turn = ((state->is_whites_turn && am_i_white) ||
                   (!state->is_whites_turn && am_i_black));

  if (is_it_my_turn) {
    let new_camera_tf =
      (state->is_whites_turn? white_camera_transform : black_camera_transform);
    swoosh_camera_to(new_camera_tf);
  }

}

void ACheckersPlayer::on_piece_clicked(AActor* piece, FKey button) {
  let state = get_gamestate();

  if (!is_it_my_turn || state->has_already_moved)
    return;

  // Calculate what cell was clicked...
  let piece_pos = piece->GetActorLocation();
  let cell = state->board->grid->calc_nearest_cell(piece_pos);
  print_to_screen("Piece clicked ("+$(cell.X)+","+$(cell.Y)+")");

  // Make sure we're allowed to play with this piece...
  if (!state->board->tile_exists(cell))
    return;
  
  bool piece_belongs_to_current_player =
    (state->board->get(cell).is_white() == state->is_whites_turn);
  if (!piece_belongs_to_current_player)
    return;
  
  // Do the actual gameplay logic...
  if (state->board->cell_has_piece(cell))
    toggle_piece_held(cell);
}

void ACheckersPlayer::on_board_clicked(AActor* some_board_mesh, FKey button) {
  let state = get_gamestate();
  
  if (!is_it_my_turn)
    return;

  // Helper functions...
  fn find_legal_move = [&](Vec2i origin, Vec2i dest) -> const CheckersMove* {
    for (let &move : *cached_moves)
      if (move.origin == origin && move.destination == dest)
        return &move;
    return nullptr;
  };

  // Calculate what cell was clicked...
  Vec location;
  Vec rotation;
  DeprojectMousePositionToWorld(location, rotation);
  let ray = Ray(location, rotation);

  Vec2i cell;
  if (!state->board->grid->intersects_ray(ray, &cell))
    PRINT_AND_THROW("HUH??? Board mesh clicked but no mouse ray intersection"
                    "with grid plane!");

  print_to_screen("Board clicked ("+$(cell.X)+","+$(cell.Y)+")");
  
  // Do the gameplay logic...
  if (!state->board->tile_exists(cell))
    return;

  // Several possibilities:
  if (state->board->cell_has_piece(cell)) {
    // - Pick up a piece... (or drop it if we're already holding it)
    if (!state->has_already_moved) {
      bool piece_belongs_to_current_player =
        (state->board->get(cell).is_white() == state->is_whites_turn);
    
      if (piece_belongs_to_current_player)
        toggle_piece_held(cell);
      
    // - If you already moved, you can still pick up or drop the piece you moved
    //   It doesn't do anything useful, but you can do it nonetheless.
    } else if (cell == state->piece_being_moved) {
      toggle_piece_held(cell);
    }
    
  // - Move the held piece...
  } else if (is_piece_held) {
    let &origin = held_piece;  // Renaming stuff for extra clarity
    let &destination = cell;
    let move = find_legal_move(origin, destination);
    
    if (move != nullptr)
      do_move(*move);
  }
}

void ACheckersPlayer::toggle_piece_held(Vec2i piece) {
  if (is_piece_held && held_piece == piece)
    drop_piece();
  else {
    drop_piece();  // Making sure the hand is empty
    grab_piece(piece);
  }
}

void ACheckersPlayer::grab_piece(Vec2i piece) {
  assert_(!is_piece_held);  // The caller has to make sure nothing is selected.
  
  var state = get_gamestate();
  
  assert_msg(!state->has_already_moved || piece == state->piece_being_moved,
    "After the first move, it is only allowed to continue with the same piece");
  
  var maybe_result = state->piece_actors.find(piece);
  if (!was_found(maybe_result, state->piece_actors))
    PRINT_AND_THROW("Piece actor not found for "+$(piece))
  
  var piece_actor = maybe_result->second;
  piece_actor->SetSelected(true);
  is_piece_held = true;
  held_piece = piece;
}

void ACheckersPlayer::drop_piece() {
  if (!is_piece_held)
    return;  // With an empty hand, this function does nothing.

  var state = get_gamestate();
  var piece = state->piece_actors.at(held_piece);
  piece->SetSelected(false);
  is_piece_held = false;
}

void ACheckersPlayer::do_move(const CheckersMove& move) {
  var state = get_gamestate();
  state->move(move.origin, move.destination);
  held_piece = move.destination;
  cached_moves = &move.followup_attacks;

  if (cached_moves->empty()) {
    state->maybe_promote(move.destination);
    state->end_turn();
  }
}

void ACheckersPlayer::swoosh_camera_to(const TF& new_camera_tf) {
  var pawn = GetPawn();
  // For some reason, SetActorTransform does not set the rotation.
  pawn->SetActorTransform(new_camera_tf);
  pawn->SetActorRotation(new_camera_tf.GetRotation());
}


ACheckersState* ACheckersPlayer::get_gamestate() {
  var state = Cast<ACheckersState>(GetWorld()->GetGameState());
  assert_(state != nullptr);
  return state;
}
