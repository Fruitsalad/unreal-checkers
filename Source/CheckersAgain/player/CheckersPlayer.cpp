#include "CheckersPlayer.h"

#include <cassert>

#include "CheckersAgain/map/Board.h"
#include "CheckersAgain/map/GridComp.h"
#include "Kismet/GameplayStatics.h"
  
ACheckersPlayer::ACheckersPlayer() { 
  static let PIECE_CLASS = get_class<AActor>(TEXT("/Game/Objects/Piece"));
  this->piece_class = PIECE_CLASS;
  assert_(this->piece_class != nullptr);
}

void ACheckersPlayer::BeginPlay() {
  var world = GetWorld();
  
  // Find the board...
  var board_actor =
    UGameplayStatics::GetActorOfClass(world, ABoard::StaticClass());
  board = Cast<ABoard>(board_actor);
  assert_msg(board != nullptr, "There must be an ABoard in the world.");
  
  // Initialize...
  board->init();
  board->prepare_default_board(board->tilesWide);
  spawn_pieces();
}

void ACheckersPlayer::spawn_pieces() {
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

    piece_actors.push_back(piece);
  });
}


