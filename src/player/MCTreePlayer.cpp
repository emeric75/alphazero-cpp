#include "MCTreePlayer.hpp"
#include <cstdlib>

//TODO : add as a parameter
int nb_sims = 1600;

MCTreePlayer::MCTreePlayer(State *init_state) 
: tree(MCTree(init_state->clone(), NULL)), cur_tree(&tree)
{}

int MCTreePlayer::play(State *s){
    //assert(s == cur_tree->cur_state);
    //std::cout << "Internal state" << std::endl;
    //cur_tree->cur_state->print();
    cur_tree->search(nb_sims);
    //std::cout << "Search stats" << std::endl;
    //cur_tree->DEBUG_print_child_stats();
    int action = cur_tree->select_action();
    register_action(action);
    return action;
}

void MCTreePlayer::register_action(int action){
    if(!cur_tree->expanded)
        cur_tree->expand();
    cur_tree = cur_tree->get_subtree_for_action(action);
}

void MCTreePlayer::reset(){
    cur_tree = &tree;
}