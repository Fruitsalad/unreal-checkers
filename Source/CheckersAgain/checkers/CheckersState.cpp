#include "CheckersState.h"
#include "Kismet/GameplayStatics.h"
#include "../map/Board.h"
#include "../map/GridComp.h"
#include "CheckersAgain/player/CheckersPlayer.h"
#include "Engine/StaticMeshActor.h"


ACheckersState::ACheckersState() : board(nullptr) {
  static let PIECE_CLASS = get_class<AActor>(TEXT("/Game/Objects/Piece"));
  piece_class = PIECE_CLASS;
  rules = INTERNATIONAL_CHECKERS;
  assert_(piece_class != nullptr);
}

void ACheckersState::BeginPlay() {
  print_to_screen("Checkers state initialized");
  var world = GetWorld();
  
  // Find the board...
  var board_actor =
    UGameplayStatics::GetActorOfClass(world, ABoard::StaticClass());
  board = Cast<ABoard>(board_actor);
  assert_msg(board != nullptr, "There must be an ABoard in the world.");
  
  // Initialize...
  board->visualizer = this;
  board->init();
  board->prepare_default_board(rules.board_width);
  spawn_pieces();
  start_turn();
}

void ACheckersState::on_piece_killed(Vec2i piece) {
  var actor = piece_actors.at(piece);
  actor->Destroy();
  piece_actors.erase(piece);
}

void ACheckersState::on_piece_promoted(Vec2i piece) {
  var actor = piece_actors.at(piece);
  actor->SetKing(true);
}

void ACheckersState::on_piece_moved(Vec2i piece, Vec2i new_position) {
  print_to_screen("Piece moved "+$(piece));
  
  // Move the actor...
  var actor = piece_actors.at(piece);
  let new_location = board->grid->calc_cell_center(new_position);
  actor->SetActorLocation(new_location);

  // Put the actor in the right place in piece_actors...
  piece_actors.erase(piece);
  piece_actors[new_position] = actor;
}

void ACheckersState::spawn_pieces() {
#define CONTINUE return
  using C = CellOccupant;
  
  assert_(board != nullptr);
  let rotation = board->GetActorRotation();
  
  board->for_each_checkers_tile([&](uint x, uint y) {
    let occupant = board->board.get(x,y).occupant;
    if (occupant == C::EMPTY)
      CONTINUE;

    let location = board->grid->calc_cell_center(x, y);
    var new_actor = GetWorld()->SpawnActor(piece_class, &location, &rotation);
    var piece = Cast<APieceBase>(new_actor);

    bool is_white = (occupant == C::WHITE || occupant == C::WHITE_KING);
    bool is_king = (occupant == C::WHITE_KING || occupant == C::BLACK_KING);
    piece->SetColor(is_white);
    piece->SetKing(is_king);
    piece->SetSelected(false);

    piece_actors[Vec2i(x,y)] = piece;
  });
}

void ACheckersState::spawn_board() {
  var world = GetWorld();
  let rotation = board->GetActorRotation();
  
  for (uint y = 0; y < board->tiles_wide; y++) {
    for (uint x = 0; x < board->tiles_wide; x++) {
      bool is_tile_black = (is_even(x) != is_even(y));
      let location = board->grid->calc_cell_center(x, y);
      //var new_actor = world->SpawnActor(board_tile_class, &location, &rotation);
      //AStaticMeshActor* new_mesh = Cast<AStaticMeshActor>(new_actor);
      //new_mesh->
    }
  }
}



// This function only actually kills opponents, but I like this name better
// than the correct name.
static void kill_everything_in_your_path(
      ABoard* board,
      Vec2i origin,
      Vec2i destination,
      bool should_kill_white) {
  using C = CellOccupant;
  
  // Helper functions...
  fn maybe_kill_opponent = [&board, should_kill_white](Vec2i pos) {
    let piece = board->get(pos);
    let is_empty = (piece.occupant == C::EMPTY);
    let is_opponent = (should_kill_white == piece.is_white());
    
    if (!is_empty && is_opponent)
      board->kill_piece(pos);
  };
  
  // Prepare...
  let diff = destination - origin;
  let step_x = ((diff.X > 0)? 1 : -1);
  let step_y = ((diff.Y > 0)? 1 : -1);
  let step = Vec2i(step_x, step_y);
  let steps_count = abs(diff.X);

  // Kill all opponents on the path...
  var cell = origin;
  for (var i = 0; i < steps_count; i += 1) {
    maybe_kill_opponent(cell);
    cell += step;
  }
}

void ACheckersState::move(Vec2i origin, Vec2i destination) {
  // We still don't know what kind of move we're making, but instead of
  // actually being smart, we just kill every opponent in the path between the
  // piece and its destination. This behavior happens to work for every kind of
  // move in the game.
  
  assert_msg(origin != destination, "That's not a move, dingus");

  has_already_moved = true;
  piece_being_moved = destination;

  let piece = board->get(origin);
  let is_white = piece.is_white();

  kill_everything_in_your_path(board, origin, destination, !is_white);
  board->move_piece(origin, destination);
}

void ACheckersState::maybe_promote(Vec2i pos) {
  let piece = board->get(pos);
  let promotion_row = (piece.is_white()? (board->tiles_wide - 1) : 0);
  
  if (!piece.is_king() && pos.Y == promotion_row)
    board->promote_piece(pos);
}


void ACheckersState::end_turn() {
  is_whites_turn = !is_whites_turn;
  start_turn();
}

void ACheckersState::start_turn() {
  let report = board->get_roundstart_report(is_whites_turn, rules);
  
  if (report.white_count == 0)
    end_game(false);
  else if (report.black_count == 0)
    end_game(true);
  else if (report.available_moves.empty())
    end_game(!is_whites_turn);
  
  longest_available_attack_chain = report.longest_available_attack_chain;
  available_moves = std::move(report.available_moves);
  has_already_moved = false;

  // Inform all player controllers about the new turn...
  TArray<APlayerController*> player_controllers;
  GEngine->GetAllLocalPlayerControllers(player_controllers);

  for (var some_kind_of_player_controller : player_controllers) {
    var player = Cast<ACheckersPlayer>(some_kind_of_player_controller);
    if (player == nullptr)
      continue;  // Not an ACheckersPlayer

    player->on_turn_started();
  }
}

void ACheckersState::end_game(bool did_white_win) {
  print_to_screen("FINALLY THE GAME IS OVER!!!", Color::Blue, 50);
  // Cause for celebration because I hate checkers.
  if (did_white_win)
    print_to_screen("WHITE WON!!!", Color::Blue, 50);
  else
    print_to_screen("BLACK WON!!!", Color::Blue, 50);
}
