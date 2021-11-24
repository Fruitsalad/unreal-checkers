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


struct RoundStartReport {
  List<Vec2i> attackers;
  bool can_move = false;
  uint white_count = 0;
  uint black_count = 0;
};

struct GridCell {
  CellOccupant occupant;
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
  UPROPERTY(VisibleAnywhere, Category=Grid) class UGridComp* grid;
  UPROPERTY(EditAnywhere, Category=Grid) unsigned tilesWide = 8;
  //UPROPERTY(EditAnywhere, Category=Checkers)
  //TScriptInterface<class IBoardVisualizer> visualizer;
  
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
  RoundStartReport get_roundstart_report(bool is_white) const;
  List<Vec2i> get_legal_moves(Vec2i piece, bool can_player_attack) const;
  void move_piece(Vec2i piece, Vec2i new_pos);  // This function is not smart.
  void kill_piece(Vec2i piece);
  void promote_piece(Vec2i piece);
};



template <class Function>
void ABoard::for_each_checkers_tile(Function do_something) const {
  for (uint y = 0; y+1 < tilesWide; y += 2) {
    // Uneven row number...
    for (uint x = 1; x < tilesWide; x += 2)
      do_something(x, y);
    
    // Even row number...
    for (uint x = 0; x < tilesWide; x += 2)
      do_something(x, y+1);
  }

  // If there is an uneven amount of rows, handle one last uneven row...
  if (tilesWide % 2 != 0) {
    uint y = tilesWide - 1;
    for (uint x = 1; x < tilesWide; x += 2)
      do_something(x, y);
  }
}
