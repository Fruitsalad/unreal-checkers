#include "CheckersState.h"
#include "Kismet/GameplayStatics.h"
#include "../map/Board.h"
#include "../map/GridComp.h"
#include "CheckersAgain/player/CheckersPlayer.h"
#include "Engine/StaticMeshActor.h"


ACheckersState::ACheckersState() : board(nullptr) {
  constant TILE_MESH = get_mesh(TEXT("/Engine/BasicShapes/Plane"));
  constant BORDER_MESH = get_mesh(TEXT("/Game/Objects/Border"));
  constant CORNER_MESH = get_mesh(TEXT("/Game/Objects/Corner"));
  constant PIECE_CLASS = get_class(TEXT("/Game/Objects/Piece"));
  constant WHITE_MAT = get_material_inst(TEXT("/Game/Materials/WhiteRamp"));
  constant BLACK_MAT = get_material_inst(TEXT("/Game/Materials/BrownRamp"));

  board_tile_mesh = TILE_MESH;
  board_border_mesh = BORDER_MESH;
  board_corner_mesh = CORNER_MESH;
  piece_class = PIECE_CLASS;
  white_tile_material = WHITE_MAT;
  black_tile_material = BLACK_MAT;
  rules = INTERNATIONAL_CHECKERS;
}

void ACheckersState::BeginPlay() {
  print_to_screen("Checkers state initialized");
  var world = GetWorld();

  board = get_actor_of_class<ABoard>(world);
  assert_msg(board != nullptr, "There must be an ABoard in the world.");
  board->visualizer = this;
  board->init();
  board->prepare_default_board(rules.board_width);

  spawn_pieces();
  spawn_board();
  init_player_controllers();
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
    let piece = board->board.get(x,y);
    if (piece.occupant == C::EMPTY)
      CONTINUE;

    let location = board->grid->calc_cell_center(x, y);
    var new_actor_ = GetWorld()->SpawnActor(piece_class, &location, &rotation);
    var new_actor = Cast<APieceBase>(new_actor_);

    new_actor->SetColor(piece.is_white());
    new_actor->SetKing(piece.is_king());
    new_actor->SetSelected(false);

    piece_actors[Vec2i(x,y)] = new_actor;
  });
}

void ACheckersState::spawn_board() {
  var world = GetWorld();
  let rotation = board->GetActorRotation();

  spawn_sides_of_board();

  // For each tile... (including the white tiles)
  for (uint y = 0; y < board->tiles_wide; y++) {
    for (uint x = 0; x < board->tiles_wide; x++) {

      // Make an actor for the tile...
      let location = board->grid->calc_cell_center(x, y);
      bool is_tile_black = (is_even(x) != is_even(y));
      var material = (is_tile_black? black_tile_material : white_tile_material);
      var new_actor =
        spawn_static_mesh(world, board_tile_mesh, location, rotation, material);

      // Give it a tag so CheckersPlayer will know that it can be clicked...
      new_actor->Tags.Add(TEXT("BOARD"));
    }
  }
}

void ACheckersState::spawn_sides_of_board() {
  // Hardcoding the offset for the meshes isn't the best but anything else is
  // probably overengineering. I just want to get this game done at this point.
  constant OFFSET = Vec(0, 0, -10);
  constant BORDER_MESH_YAW = Rot(0, 90, 0);

  var world = GetWorld();
  let board_rotation = board->GetActorRotation();
  let w = rules.board_width;

  constant CORNER_TILES_SIZE = 4;
  Vec2i corner_tiles[CORNER_TILES_SIZE] = {
    Vec2i(-1, w),
    Vec2i(-1, -1),
    Vec2i(w, -1),
    Vec2i(w, w)
  };

  // For each of the four sides...
  Vec corner_loc =
      board->grid->calc_cell_center(corner_tiles[0]) + OFFSET;
  for (uint i = 0; i < CORNER_TILES_SIZE; i++) {

    // Calculate the rotation...
    let yaw = i * 90;  // Unreal uses degrees instead of radians apparently
    let rotation = board_rotation + Rot(0, yaw, 0);

    // Calculate the world position of the border mesh...
    // (It should be exactly in the middle of this side)
    let next_i = (i + 1) % CORNER_TILES_SIZE;
    Vec next_corner_loc =
        board->grid->calc_cell_center(corner_tiles[next_i]) + OFFSET;
    Vec border_loc = (next_corner_loc - corner_loc) / 2.f + corner_loc;

    // Spawn in the meshes... (one corner & one straight border)
    spawn_static_mesh(
        world, board_corner_mesh,
        corner_loc, rotation, black_tile_material
    );
    var border = spawn_static_mesh(
        world, board_border_mesh,
        border_loc, rotation + BORDER_MESH_YAW, black_tile_material
    );

    // Scale the straight border mesh so it's as wide as the playing area...
    var transform = border->GetActorTransform();
    transform.SetScale3D(Vec(rules.board_width, 1, 1));
    border->SetActorTransform(transform);

    // We carry next_corner_loc over to corner_loc for efficiency, otherwise we
    // would calculate it twice.
    corner_loc = next_corner_loc;
  }
}


template<class DoSomething>
static void for_each_player(DoSomething do_something) {
  TArray<APlayerController*> player_controllers;
  GEngine->GetAllLocalPlayerControllers(player_controllers);

  for (var some_kind_of_player_controller : player_controllers) {
    var player = Cast<ACheckersPlayer>(some_kind_of_player_controller);
    if (player != nullptr)  // If this is null, it's not an ACheckersPlayer
      do_something(player);
  }
}


void ACheckersState::init_player_controllers() {
  for_each_player([](ACheckersPlayer* player) {
    player->initialize();
  });
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
  for_each_player([](ACheckersPlayer* player) {
    player->on_turn_started();
  });
}

void ACheckersState::end_game(bool did_white_win) {
  print_to_screen("FINALLY THE GAME IS OVER!!!", Color::Blue, 50);
  // Cause for celebration because I hate checkers.
  if (did_white_win)
    print_to_screen("WHITE WON!!!", Color::Blue, 50);
  else
    print_to_screen("BLACK WON!!!", Color::Blue, 50);
}
