#include "Board.h"

#include "BoardVisualizer.h"
#include "GridComp.h"



ABoard::ABoard() {
  PrimaryActorTick.bCanEverTick = false;

  RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
  grid = CreateDefaultSubobject<UGridComp>(TEXT("Grid Component"));
}

void ABoard::init() {
  prepare_empty_board(tiles_wide);
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
  tiles_wide = new_width;
  board.reset(tiles_wide, tiles_wide);
  for (var &cell : board)
    cell.occupant = CellOccupant::EMPTY;
}

void ABoard::prepare_default_board(uint new_width) {
  tiles_wide = new_width;
  board.reset(tiles_wide, tiles_wide);
  
  var top_empty_row = tiles_wide / 2;
  let bottom_empty_row = top_empty_row - 1;

  // If there is an uneven amount of rows, then we make three empty rows...
  if (!is_even(tiles_wide))
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


static List<CheckersMove> get_legal_attacks__impl(
    const ABoard* board, Vec2i start_pos, bool is_white, bool is_king,
    const List<Vec2i>& newly_empty_tiles) {
  // This function is recursive because when we find a valid attack, we need
  // to see if we can find valid follow-up attacks. It's easiest to do that
  // with a recursive function.
  
  using C = CellOccupant;


  fn get_occupant = [&](Vec2i tile) -> CellOccupant {
    // For follow-up attacks, we want to make sure we're not trying to kill the
    // same piece twice.
    for (let &newly_empty_tile : newly_empty_tiles)
      if (newly_empty_tile == tile)
        return CellOccupant::EMPTY;

    return board->get(tile).occupant;
  };
  
  
  List<CheckersMove> result;
  
  // For each direction...
  let &dirs = (is_king? king_dirs : (is_white? white_dirs : black_dirs));
  let dir_count = (is_king? king_dir_count : normal_dir_count); 
  
  for (uint i = 0; i < dir_count; i++) {
    let dir = dirs[i];

    // Get the victim and the victim's neighbor...
    // The victim is the piece that this attack will kill.
    var victim = start_pos + one_step[dir];
    // The victim's neighbor is the tile that we will move to with this attack.
    var victims_neighbor = victim + one_step[dir];

    // Check whether the neighbor exists, so we don't go over the edge of the
    // playing board... (if the victim's neighbor exists, the victim also
    // exists)
    if (!board->tile_exists(victims_neighbor))
      continue; 
    
    // We can't attack an empty neighboring tile...
    var victim_occupant = get_occupant(victim);
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

        victim_occupant = get_occupant(victim);
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

    // Check if it's a valid attack...
    let is_victim_white = (victim_occupant == C::WHITE ||
                           victim_occupant == C::WHITE_KING);
    let can_attack = (is_white != is_victim_white);  // Must be different colors
    let is_victims_neighbor_empty =
        (get_occupant(victims_neighbor) == C::EMPTY);

    if (can_attack && is_victims_neighbor_empty) {
      // This is a valid attack!
      
      // A linked list might be more efficient here but honestly I can't be
      // bothered.
      let new_pos = victims_neighbor;
      var killed_list = newly_empty_tiles;  // Yep. Just copying the whole thing
      killed_list.push_back(victim);
      var followup_moves =
        get_legal_attacks__impl(board, new_pos, is_white, is_king, killed_list);

      // Put together the CheckersMove datastructure...
      CheckersMove new_move;
      new_move.origin = start_pos;
      new_move.destination = victims_neighbor;
      var &max_length = new_move.longest_attack_chain_length;  // Alias
      max_length = 1;

      for (var &followup : followup_moves) {
        let new_length = followup.longest_attack_chain_length + 1;
        if (new_length > max_length)
          max_length = new_length;
        
        new_move.followup_attacks.push_back(move(followup));
      }
      
      result.push_back(move(new_move));
    }
  }

  return result;
}


static List<CheckersMove> get_legal_attacks(const ABoard* board, Vec2i pos) {
  let piece = board->get(pos);
  let is_white = piece.is_white();
  let is_king = piece.is_king();
  return get_legal_attacks__impl(board, pos, is_white, is_king, {pos});
}


static List<CheckersMove> get_legal_normal_moves(
      const ABoard* board,
      Vec2i pos) {
  using C = CellOccupant;

  let piece = board->get(pos);
  let is_white = piece.is_white();
  let is_king = piece.is_king();

  List<CheckersMove> result;
  
  // For each direction...
  let &dirs = (is_king? king_dirs : (is_white? white_dirs : black_dirs));
  let dir_count = (is_king? king_dir_count : normal_dir_count); 
  
  for (uint i = 0; i < dir_count; i++) {
    let dir = dirs[i];

    // Get the neighbor...
    let neighbor = pos + one_step[dir];

    if (!board->tile_exists(neighbor))
      continue;

    // If it's empty, this is a valid move...
    if (board->get(neighbor).occupant == C::EMPTY)
      result.push_back(CheckersMove{pos, neighbor, 0, {}});

    // Unless we're a flying king, that's the only possible move in this
    // direction...
    if (!is_king)
      continue;

    // For flying kings we also check all the tiles behind the neighbor...
    var next_tile = neighbor + one_step[dir];
    while (board->tile_exists(next_tile)
           && board->get(next_tile).occupant == C::EMPTY) {
      result.push_back(CheckersMove{pos, next_tile, 0, {}});
      next_tile += one_step[dir];
    }
  }

  return result;
}


static void cull_illegal_moves__impl(
    List<CheckersMove>* moves,
    uint desired_length) {
  if (moves->empty())
    return;
  
  for (var iter = moves->begin(); iter != moves->end(); ) {
    if (iter->longest_attack_chain_length < desired_length)
      iter = moves->erase(iter);
    else {
      cull_illegal_moves__impl(&iter->followup_attacks, desired_length - 1);
      ++iter;
    }
  }

  // If the list of moves wasn't empty when we started, at least one move should
  // survive the culling.
  assert_(!moves->empty());
}


static void cull_illegal_moves(RoundStartReport& report) {
  let longest = report.longest_available_attack_chain;
  cull_illegal_moves__impl(&report.available_moves, longest);
}


RoundStartReport ABoard::get_roundstart_report(bool is_player_white) const {
# define CONTINUE return
  using C = CellOccupant;
  
  RoundStartReport report;
  var &longest_chain = report.longest_available_attack_chain;  // alias
  
  for_each_checkers_tile([&](uint x, uint y) {
    let piece = board.get(x,y);

    // Count white and black pieces, and skip to the next tile if this isn't a
    // piece belonging to the current player...
    if (piece.is_white()) {
      report.white_count += 1;
      if (!is_player_white)
        CONTINUE;
    } else if (piece.occupant != C::EMPTY) {
      report.black_count += 1;
      if (is_player_white)
        CONTINUE;
    } else CONTINUE;

    // Create a list of possible moves and attacks...
    // This way of doing things is not very efficient, but it works!
    let pos = Vec2i(x,y);
    
    for (var &attack : get_legal_attacks(this, pos)) {
      let chain_length = attack.longest_attack_chain_length;
      if (chain_length > longest_chain)
        longest_chain = chain_length;
      
      report.available_moves.push_back(move(attack));
    }
    
    for (var &normal_move : get_legal_normal_moves(this, pos))
      report.available_moves.push_back(move(normal_move));
  });

  cull_illegal_moves(report);

  return report;
# undef CONTINUE
}


void ABoard::move_piece(Vec2i old_pos, Vec2i new_pos) {
  let &new_tile = get(new_pos);
  var &old_tile = get(old_pos);
  assert_(new_tile.occupant == CellOccupant::EMPTY);
  get(new_pos).occupant = old_tile.occupant;
  old_tile.occupant = CellOccupant::EMPTY;

  if (visualizer)
    visualizer->on_piece_moved(old_pos, new_pos);
}

void ABoard::kill_piece(Vec2i pos) {
  get(pos).occupant = CellOccupant::EMPTY;

  if (visualizer)
    visualizer->on_piece_killed(pos);
}

void ABoard::promote_piece(Vec2i pos) {
  var &tile = get(pos);
  assert_(tile.occupant != CellOccupant::EMPTY);
  
  if (tile.occupant == CellOccupant::WHITE)
    tile.occupant = CellOccupant::WHITE_KING;
  else if (tile.occupant == CellOccupant::BLACK)
    tile.occupant = CellOccupant::BLACK_KING;
  else return;

  if (visualizer)
    visualizer->on_piece_promoted(pos);
}

bool ABoard::cell_has_piece(Vec2i pos) const {
  return get(pos).occupant != CellOccupant::EMPTY;
}



