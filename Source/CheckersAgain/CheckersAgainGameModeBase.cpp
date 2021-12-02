#include "CheckersAgainGameModeBase.h"

#include "player/CheckersPlayer.h"

ACheckersAgainGameModeBase::ACheckersAgainGameModeBase() {
  PlayerControllerClass = ACheckersPlayer::StaticClass();
  DefaultPawnClass = get_class<APawn>(TEXT("/Game/Objects/Player"));
}
