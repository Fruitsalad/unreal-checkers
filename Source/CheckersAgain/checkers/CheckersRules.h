#pragma once

#include "../util.hpp"


/// In checkers there are usually multiple moves possible in a turn and players
/// can choose which move they wish to make. All variants of checkers limit
/// the player's freedom to choose in some way,
/// for example in international checkers you are only allowed to make a move
/// that leads to the longest attack chain in that turn (so when you can take
/// two pieces in a turn but it leads to a disadvantageous position,
/// and all of the other moves would only lead to one or zero pieces being
/// taken, then you nonetheless must play that move even if it's
/// disadvantageous).
enum class ChoiceFilterRule {
  LONGEST_CHAIN_ONLY
};


struct CheckersRules {
  uint board_width = 10;

  // In some rulesets normal pieces can't attack backwards, eg English draughts
  bool can_all_pieces_attack_backwards = true;

  // A flying king can move and attack at any distance (and not just one square)
  bool can_king_fly = true;

  // If false, a flying king must stop on the square behind its victim.
  bool can_king_fly_after_kill = true;

  // If true, a piece can only be promoted if it gets to the last row at the
  // natural end of a turn (so if it can still attack but that attack moves it
  // out of the promotion row, it has to attack and won't be promoted)
  bool can_promote_only_at_end_of_turn = true;

  // This is relevant in case can_promote_only_at_end_of_turn is false. If this
  // is false, then a promotion basically automatically ends a turn, even if
  // there is still an attack possible. If this is true, a piece can be promoted
  // in the middle of a turn.
  bool can_continue_after_promotion = false;

  // In the case of a flying king, it is possible that the king is prevented
  // from making a second attack because a killed piece is blocking him.
  bool can_killed_piece_block_further_attacks = true;

  // See comments for ChoiceFilterRule.
  ChoiceFilterRule filter_rule = ChoiceFilterRule::LONGEST_CHAIN_ONLY;
};


const inline CheckersRules INTERNATIONAL_CHECKERS = {};
