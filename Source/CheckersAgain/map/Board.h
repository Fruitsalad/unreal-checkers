#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Array2D.hpp"
#include "Board.generated.h"


enum class CellOccupant {
  EMPTY,
  WHITE,
  BLACK,
  WHITE_KING,
  BLACK_KING
};

enum Direction {
  NE = 0,
  SE = 1,
  SW = 2,
  NW = 3
};

struct CheckersMove {
  Vec2i origin;
  Vec2i destination;
  uint longest_attack_chain_length;
  // I could imagine that the memory usage of storing all possible attack chains
  // could go slightly out of control if we have a big board with an unfortunate
  // placement of pieces. Hopefully it won't be an issue in practice.
  List<struct CheckersMove> followup_attacks;
};

struct RoundStartReport {
  // Note: available_moves does not take the attack chain length into
  // consideration. For example you normally can't do a normal move when it is
  // possible to attack, but normal moves will still be listed in
  // available_moves nonetheless.
  List<CheckersMove> available_moves;
  uint white_count = 0;
  uint black_count = 0;
  uint longest_available_attack_chain = 0;
};

struct GridCell {
  CellOccupant occupant;

  bool is_white() const {
    return occupant == CellOccupant::WHITE ||
           occupant == CellOccupant::WHITE_KING;
  }

  bool is_king() const {
    return occupant == CellOccupant::WHITE_KING ||
           occupant == CellOccupant::BLACK_KING;
  }
};


// This can be used to get the coordinate of the neighbor of a tile.
// The index should be a Direction.
const inline Vec2i one_step[] = {{-1,1},{-1,-1},{1,-1},{1,1}};

inline const Direction white_dirs[] = {NE, NW};
inline const Direction black_dirs[] = {SE, SW};
inline const Direction king_dirs[] = {NE, SE, SW, NW};
inline const uint normal_dir_count = 2;
inline const uint king_dir_count = 4;

template<class Function>
void for_each_direction(Function do_something) {
  for (Direction dir = 0; dir < 4; dir++)
    do_something(dir);
}


UCLASS()
class CHECKERSAGAIN_API ABoard : public AActor {
  GENERATED_BODY()
public:
  UPROPERTY(VisibleAnywhere, Category=Grid)
  class UGridComp* grid;
  UPROPERTY(EditAnywhere, Category=Grid)
  unsigned tiles_wide = 8;
  
  struct I_BoardVisualizer* visualizer = nullptr;
	Array2D<GridCell> board;
  
  ABoard();

  void init();

  // do_something: (x:uint, y:uint) -> void
  template<class Function>
  void for_each_checkers_tile(Function do_something) const;

  GridCell& get(Vec2i pos);
  const GridCell& get(Vec2i pos) const;
  bool tile_exists(Vec2i pos) const;
  void prepare_empty_board(uint new_width);
  void prepare_default_board(uint new_width);
  RoundStartReport get_roundstart_report(bool is_player_white) const;
  void move_piece(Vec2i old_pos, Vec2i new_pos);  // This function is not smart.
  void kill_piece(Vec2i pos);
  void promote_piece(Vec2i pos);
  bool cell_has_piece(Vec2i pos) const;
};



template <class Function>
void ABoard::for_each_checkers_tile(Function do_something) const {
  for (uint y = 0; y+1 < tiles_wide; y += 2) {
    // Uneven row number...
    for (uint x = 1; x < tiles_wide; x += 2)
      do_something(x, y);
    
    // Even row number...
    for (uint x = 0; x < tiles_wide; x += 2)
      do_something(x, y+1);
  }

  // If there is an uneven amount of rows, handle one last uneven row...
  if (tiles_wide % 2 != 0) {
    uint y = tiles_wide - 1;
    for (uint x = 1; x < tiles_wide; x += 2)
      do_something(x, y);
  }
}
