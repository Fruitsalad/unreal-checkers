#include "CheckersMode.h"
#include "CheckersState.h"
#include "../player/CheckersPlayer.h"
#include "../util.hpp"


ACheckersMode::ACheckersMode() {
  PlayerControllerClass = ACheckersPlayer::StaticClass();
  DefaultPawnClass = get_class<APawn>(TEXT("/Game/Objects/Player"));
  GameStateClass = ACheckersState::StaticClass();
}
