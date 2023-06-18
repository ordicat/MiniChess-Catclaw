#include <cstdlib>

#include "../state/state.hpp"
#include "./minmax.hpp"

#define MAXDEPTH 3

/**
 * @brief Get an action from MinMax algorithm
 * 
 * @param state Now state
 * @param depth You may need this for other policy
 * @return Move 
 */
Move MinMax::get_move(State *state, int depth){
  // placeholder code here. it is purely state-value for now!
    if(!state->legal_actions.size())
    state->get_legal_actions();
    Move bestmove;
    int maxeval = 0;
    for(auto it : state->legal_actions){
        State *tempmove = state->next_state(it);
        int tempeval = -(tempmove->evaluate());
        if(tempeval > maxeval){ //keep oldest value
            bestmove = it;
            maxeval = tempeval;
        }
    }
    return bestmove;
}