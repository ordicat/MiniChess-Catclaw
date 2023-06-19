#pragma once
#include "../state/state.hpp"


/**
 * @brief Policy class for MinMax policy, 
 * your policy class should have get_move method
 */
class MinMax{
public:
  static Move get_move(State *state, int depth);
private:
  static int minimax(State *state, int depth, bool max);
};