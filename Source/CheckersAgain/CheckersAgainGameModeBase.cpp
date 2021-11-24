#include "CheckersAgainGameModeBase.h"

#include "player/CheckersPlayer.h"

ACheckersAgainGameModeBase::ACheckersAgainGameModeBase() {
  PlayerControllerClass = ACheckersPlayer::StaticClass();
}
