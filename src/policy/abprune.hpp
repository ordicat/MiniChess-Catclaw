#pragma once
#include "../state/state.hpp"


/**
 * @brief Policy class for ABpruning policy,
 * which is a clone of the MiniMax alg. 
 * your policy class should have get_move method
 */
class ABprune{
public:
  static Move get_move(State *state, int depth);
private:
  static int alphabeta(State *state, int depth, int alpha, int beta, bool max);
};