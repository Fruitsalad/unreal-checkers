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

void ACheckersPlayer::BeginPlay() {
  make_board_clickable();
  make_pieces_clickable();
}

void ACheckersPlayer::bind_inputs() {
  assert_(InputComponent);
}

void ACheckersPlayer::make_pieces_clickable() {
  for (var [position, piece_actor] : get_gamestate()->piece_actors)
    piece_actor->OnClicked.AddDynamic(this, &This::on_piece_clicked);
}

void ACheckersPlayer::make_board_clickable() {
  for (AActor* actor : actors_in_world(GetWorld()))
    if (actor->ActorHasTag(TEXT("BOARD")))
      actor->OnClicked.AddDynamic(this, &This::on_board_clicked);
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
  
  if (!is_it_my_turn || state->has_already_moved)
    return;

  // Helper functions... (boring)
  fn is_legal_move = [&](Vec2i destination) {
    for (let &move : cached_legal_moves)
      if (move == destination)
        return true;
    return false;
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
  // - Pick up a piece... (or drop it if we're already holding it)
  if (state->board->cell_has_piece(cell)) {
    bool piece_belongs_to_current_player =
        (state->board->get(cell).is_white() == state->is_whites_turn);
    if (piece_belongs_to_current_player)
      toggle_piece_held(cell);
    
  // - Move the held piece...
  } else if (is_piece_held) {
    let &destination = cell;  // Alias for clarity
    if (is_legal_move(destination))
      commit_move(held_piece, destination);
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
  
  var maybe_result = state->piece_actors.find(piece);
  if (!was_found(maybe_result, state->piece_actors))
    PRINT_AND_THROW("Piece actor not found for "+$(piece))
  
  var piece_actor = maybe_result->second;
  piece_actor->SetSelected(true);
  is_piece_held = true;
  held_piece = piece;

  // Update the cached legal moves...
  cached_legal_moves =
    state->board->get_legal_moves(piece, state->can_player_attack);
}

void ACheckersPlayer::drop_piece() {
  if (!is_piece_held)
    return;  // With an empty hand, this function does nothing.

  var state = get_gamestate();
  var piece = state->piece_actors.at(held_piece);
  piece->SetSelected(false);
  is_piece_held = false;
}

void ACheckersPlayer::commit_move(Vec2i piece, Vec2i destination) {
  var state = get_gamestate();
  drop_piece();
  state->commit_move(piece, destination);
}

ACheckersState* ACheckersPlayer::get_gamestate() {
  var state = Cast<ACheckersState>(GetWorld()->GetGameState());
  assert_(state != nullptr);
  return state;
}
