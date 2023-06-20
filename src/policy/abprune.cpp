#include <cstdlib>
#include <algorithm>
#include <set>
#include <iostream>

#include "../state/state.hpp"
#include "./abprune.hpp"

#define INF 2147483647

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

int ABprune:: alphabeta(State *state, int depth, int alpha, int beta, bool max){
    if(depth == 0 || state->game_state != NONE) return state->evaluate();
    if(max){
        int value = -2147483648;
        for(auto it : state->legal_actions){
            value = std::max(value, alphabeta(state->next_state(it), depth-1, alpha, beta, false));
            alpha = std::max(alpha, value);
            if(alpha >= beta) break;
        }
        return value;
    }
    else{
        int value = 2147483647;
        for(auto it : state->legal_actions){
            value = std::min(value, alphabeta(state->next_state(it), depth-1, alpha, beta, true));
            beta = std::min(beta, value);
            if(beta <= alpha) break;
        }
        return value;
    }
}

Move ABprune::get_move(State *state, int depth){
    std::set<statequeue> beststate;
    if(state->legal_actions.empty()) state->get_legal_actions();
    for(auto it = state->legal_actions.begin(); it != state->legal_actions.end(); it++){
        statequeue temp;
        temp.move = *it;
        temp.heuristic = alphabeta(state->next_state(temp.move), depth, -INF, INF, false);//we want the opponent to have the worst move after we move
        beststate.insert(temp);
    }
    
    Move bestmove = nomove;
    for(auto states = beststate.begin(); states != beststate.end(); states++){
        State* tempstate = state-> next_state(states->move);
        if(tempstate->game_state != WIN) {
            bestmove = states->move;
            break;
        }
    }
    //failsafe: if queue is empty or best move is no move then do a random move 
    if(bestmove == nomove || beststate.empty()){
        auto actions = state->legal_actions;
        return actions[(rand()+depth)%actions.size()];
    }
    return bestmove;
}
