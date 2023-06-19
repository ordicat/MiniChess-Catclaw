#include <cstdlib>
#include <algorithm>
#include <queue>
#include <iostream>

#include "../state/state.hpp"
#include "./minmax.hpp"

/**
 * @brief Get an action from MinMax algorithm
 * 
 * @param state Now state
 * @param depth You may need this for other policy
 * @return Move 
 */
const Move nomove = {{0,0},{0,0}};

struct statequeue{
    Move move;
    int heuristic;
    friend bool operator<(const statequeue& lhs, const statequeue& rhs) {
        return lhs.heuristic < rhs.heuristic;
    }
};

int MinMax:: minimax(State *state, int depth, bool max){
    if(depth == 0 || state->game_state != NONE) return state->evaluate();
    if(max){
        int value = -2147483648;
        for(auto it : state->legal_actions)
            value = std::max(value, minimax(state->next_state(it), depth-1, false));
        return value;
    }
    else{
        int value = 2147483647;
        for(auto it : state->legal_actions)
            value = std::min(value, minimax(state->next_state(it), depth-1, true));
        return value;
    }
}

Move MinMax::get_move(State *state, int depth){
    std::priority_queue<statequeue> beststate;
    if(state->legal_actions.empty()) state->get_legal_actions();
    for(auto it = state->legal_actions.begin(); it != state->legal_actions.end(); it++){
        statequeue temp;
        temp.move = *it;
        temp.heuristic = minimax(state->next_state(temp.move), depth, true);
        beststate.push(temp);
    }
    
    Move bestmove = beststate.top().move;
    //failsafe: if queue is empty or best move is no move then do a random move 
    if(bestmove == nomove || beststate.empty()){
    //if(true){//testing output for now
        auto actions = state->legal_actions;
        return actions[(rand()+depth)%actions.size()];
    }
    return bestmove;
}
