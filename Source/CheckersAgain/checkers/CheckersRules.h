#pragma once

#include "../util.hpp"


/// Usually in checkers, it is possible to make multiple different kinds of
/// moves. All variants of checkers limit which moves you're allowed to take,
/// for example in international checkers you are only allowed to make a move
/// that leads to the longest attack chain in that turn (i.e. when you can take
/// two pieces in a turn but it leads to a disadvantageous position, you
/// nonetheless must play that move, unless there's other moves that let you
/// take two pieces).
enum class ChoiceFilterRule {
  LONGEST_CHAIN_ONLY
};


struct CheckersRules {
  uint board_width = 10;
  bool can_all_pieces_attack_backwards = true;
  bool can_king_fly = true;
  bool can_king_fly_after_kill = true;
  bool can_promote_only_at_end_of_turn = true;
  // This is relevant in case promote_only_at_end_of_turn is false:
  bool does_promotion_end_chain = false;
  // In the case of a flying king, it is possible that the king is prevented
  // from making a second attack because a killed piece is blocking him.
  bool can_killed_piece_block_further_attacks = true;
  ChoiceFilterRule filter_rule = ChoiceFilterRule::LONGEST_CHAIN_ONLY;
};


const inline CheckersRules INTERNATIONAL_CHECKERS = {};
