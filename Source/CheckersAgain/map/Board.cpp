#include "Board.h"
#include "GridComp.h"
#include "BoardVisualizer.h"



// For some reason Unreal does not provide these by default.
static Vec2i operator + (const Vec2i& a, const Vec2i& b) {
  return {a.X + b.X, a.Y + b.Y};
}

static Vec2i& operator += (Vec2i& a, const Vec2i& b) {
  a.X += b.X;
  a.Y += b.Y;
  return a;
}



ABoard::ABoard() {
  PrimaryActorTick.bCanEverTick = false;

  RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
  grid = CreateDefaultSubobject<UGridComp>(TEXT("Grid Component"));
}

void ABoard::init() {
  prepare_empty_board(tilesWide);
}

GridCell& ABoard::get(Vec2i pos) {
  return board.get(pos.X, pos.Y);
}

const GridCell& ABoard::get(Vec2i pos) const {
  return board.get(pos.X, pos.Y);
}

bool ABoard::tile_exists(Vec2i pos) const {
  return board.is_valid(pos.X, pos.Y);
}

void ABoard::prepare_empty_board(uint new_width) {
  tilesWide = new_width;
  board.reset(tilesWide, tilesWide);
  for (var &cell : board)
    cell.occupant = CellOccupant::EMPTY;
}

void ABoard::prepare_default_board(uint new_width) {
  tilesWide = new_width;
  board.reset(tilesWide, tilesWide);
  
  var top_empty_row = tilesWide / 2;
  let bottom_empty_row = top_empty_row - 1;

  // If there is an uneven amount of rows, then we make three empty rows...
  if (!is_even(tilesWide))
    top_empty_row += 1;

  for_each_checkers_tile([&](uint x, uint y) {
    if (y < bottom_empty_row)
      board.get(x,y).occupant = CellOccupant::WHITE;
    else if (y > top_empty_row)
      board.get(x,y).occupant = CellOccupant::BLACK;
    else
      board.get(x,y).occupant = CellOccupant::EMPTY;
  });
}



static List<Vec2i> get_legal_attacks(const ABoard* board, Vec2i piece) {
  using C = CellOccupant;

  List<Vec2i> result;

  let occupant = board->get(piece).occupant;
  let is_white = (occupant == C::WHITE || occupant == C::WHITE_KING);
  let is_king = (occupant == C::WHITE_KING || occupant == C::BLACK_KING);
  
  // For each direction...
  let dirs = (is_king? king_dirs : (is_white? white_dirs : black_dirs));
  let dir_count = (is_king? king_dir_count : normal_dir_count); 
  
  for (uint i = 0; i < dir_count; i++) {
    let dir = dirs[i];

    // Get the victim and the victim's neighbor...
    // The victim is the piece that this attack will kill.
    var victim = piece + one_step[dir];
    // The victim's neighbor is the tile that we will move to with this attack.
    var victims_neighbor = victim + one_step[dir];

    // Check whether the neighbors exist, so we don't go over the edge of the
    // playing board... (if the neighbor's neighbor exists, the neighbor also
    // exists)
    if (!board->tile_exists(victims_neighbor))
      continue; 
    
    // We can't attack an empty neighboring tile...
    var victim_occupant = board->get(victim).occupant;
    if (victim_occupant == C::EMPTY) {
      // If the neighbor is empty and this is a normal piece, we're done...
      if (!is_king)
        continue;

      // If we're a king we look for the first non-empty piece and make that our
      // victim... (flying king rule)
      bool found_victim = false;
      
      for (;;) {
        victim = victims_neighbor;
        victims_neighbor += one_step[dir];
        if (!board->tile_exists(victims_neighbor))
          break;

        victim_occupant = board->get(victim).occupant;
        if (victim_occupant != C::EMPTY) {
          found_victim = true;
          break;
        }
      }
      
      if (!found_victim)
        continue;
    }

    // We now have a non-empty victim, and the victim's neighbor is known
    // to exist.

    // If the neighbor can be attacked...
    let is_neighbor_black = (victim_occupant == C::BLACK ||
                             victim_occupant == C::BLACK_KING);

    if (is_white && is_neighbor_black || !is_white && !is_neighbor_black) {
      // And the neighbor's neighbor is empty...
      if (board->get(victims_neighbor).occupant == C::EMPTY)
        // Then this is a valid attack.
        result.push_back(victims_neighbor);
    }
  }

  return result;
}


static List<Vec2i> get_legal_normal_moves(const ABoard* board, Vec2i piece) {
  using C = CellOccupant;

  let occupant = board->get(piece).occupant;
  let is_white = (occupant == C::WHITE || occupant == C::WHITE_KING);
  let is_king = (occupant == C::WHITE_KING || occupant == C::BLACK_KING);

  List<Vec2i> result;
  
  // For each direction...
  let dirs = (is_king? king_dirs : (is_white? white_dirs : black_dirs));
  let dir_count = (is_king? king_dir_count : normal_dir_count); 
  
  for (uint i = 0; i < dir_count; i++) {
    let dir = dirs[i];

    // Get the neighbor...
    let neighbor = piece + one_step[dir];

    // If it's empty, this is a valid move...
    if (board->get(neighbor).occupant == C::EMPTY)
      result.push_back(neighbor);

    // Unless we're a flying king, that's the only possible move in this
    // direction...
    if (!is_king)
      continue;

    // For flying kings we also check all the tiles behind the neighbor...
    var next_tile = neighbor + one_step[dir];
    while (board->tile_exists(next_tile)
           && board->get(next_tile).occupant == C::EMPTY) {
      result.push_back(next_tile);
      next_tile += one_step[dir];
    }
  }

  return result;
}


RoundStartReport ABoard::get_roundstart_report(bool is_white) const {
# define CONTINUE return
  using C = CellOccupant;
  
  RoundStartReport result;
  
  for_each_checkers_tile([&](uint x, uint y) {
    let occupant = board.get(x,y).occupant;

    // Count white and black pieces, and skip to the next tile if this isn't a
    // piece belonging to the current player...
    if (occupant == C::WHITE) {
      result.white_count += 1;
      if (!is_white)
        CONTINUE;
    } else if (occupant == C::BLACK) {
      result.black_count += 1;
      if (is_white)
        CONTINUE;
    } else CONTINUE;

    // Check if this piece can move or attack...
    // This implementation is not very efficient, but it works!
    let piece = Vec2i(x,y);
    
    if (!get_legal_attacks(this, piece).empty()) {
      result.attackers.push_back(piece);
      result.can_move = true;
    } else if (!result.can_move &&
        !get_legal_normal_moves(this, piece).empty()) {
      result.can_move = true;
    }
  });

  return result;
# undef CONTINUE
}

List<Vec2i> ABoard::get_legal_moves(Vec2i piece, bool can_player_attack) const {
  if (can_player_attack)
    return get_legal_attacks(this, piece);
  else
    return get_legal_normal_moves(this, piece);
}

void ABoard::move_piece(Vec2i piece, Vec2i new_pos) {
  let &new_tile = get(new_pos);
  var &old_tile = get(piece);
  assert_(new_tile.occupant == CellOccupant::EMPTY);
  get(new_pos).occupant = old_tile.occupant;
  old_tile.occupant = CellOccupant::EMPTY;
}

void ABoard::kill_piece(Vec2i piece) {
  get(piece).occupant = CellOccupant::EMPTY;
}

void ABoard::promote_piece(Vec2i piece) {
  var &tile = get(piece);
  assert_(tile.occupant != CellOccupant::EMPTY);
  if (tile.occupant == CellOccupant::WHITE)
    tile.occupant = CellOccupant::WHITE_KING;
  else if (tile.occupant == CellOccupant::BLACK)
    tile.occupant = CellOccupant::BLACK_KING;
}




