#pragma once

#include "CoreMinimal.h"
#include "../util.hpp"


struct I_BoardVisualizer {
  virtual ~I_BoardVisualizer() = default;
  virtual void on_piece_killed(Vec2i piece) {}
  virtual void on_piece_moved(Vec2i piece, Vec2i new_position) {}
  virtual void on_piece_promoted(Vec2i piece) {}
};
