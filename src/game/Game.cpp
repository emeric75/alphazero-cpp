#include "Player.hpp"
#include "State.hpp"
#include <iostream>

//Returns 0, 1, 2 (draw, 1 wins, 2 wins)
int play(Player &p1, Player &p2, State *init_state, bool verbose=false){
    p1.reset();
    p2.reset();
    State *cur_state = init_state;
    State *tmp_state;
    int cur_player = 1;
    while(cur_state->eval() == -1){
        if(verbose) cur_state->print();
        if(cur_player == 1){
            if(verbose) std::cout << "Player 1's turn!" << std::endl;
            int action = p1.play(cur_state);
            if(verbose) std::cout << "\tPlayer 1's played action " << action << std::endl;
            
            tmp_state = cur_state->nextState(action);
            if(cur_state != init_state) delete cur_state;
            cur_state = tmp_state;

            p2.register_action(action);
            cur_player = 2;
        }else{//cur_player == 2
            if(verbose) std::cout << "Player 2's turn!" << std::endl;
            int action = p2.play(cur_state);
            if(verbose) std::cout << "\tPlayer 2's played action " << action << std::endl;
            
            tmp_state = cur_state->nextState(action);
            if(cur_state != init_state) delete cur_state;
            cur_state = tmp_state;
            
            p1.register_action(action);
            cur_player = 1;
        }
    }
    int res = (int) cur_state->eval();
    if (verbose){
        if(res == 0){
            std::cout << "Draw!" << std::endl;
        }else{
            std::cout << "Player " << res << " won!" << std::endl;
        }
    }
    if(cur_state != init_state) delete cur_state;
    return res;
}