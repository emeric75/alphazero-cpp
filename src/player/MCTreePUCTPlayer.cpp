#include "MCTreePUCTPlayer.hpp"
#include "InitPredictor.hpp"
#include "MCTreePUCT.hpp"
#include <cstdlib>
#include <utility>

//TODO : add as a parameter
int nb_sims = 200;

MCTreePUCTPlayer::MCTreePUCTPlayer(State *init_state, InitPredictor &pred) 
: tree(MCTreePUCT(init_state->clone(), pred, NULL)), 
  cur_tree(&tree)
{}

int MCTreePUCTPlayer::play(State *s){
    //assert(s == cur_tree->cur_state);
    //std::cout << "Internal state" << std::endl;
    //cur_tree->cur_state->print();
    cur_tree->search(nb_sims);
    //std::cout << "Search stats" << std::endl;
    //cur_tree->DEBUG_print_child_stats();
    int action = cur_tree->select_action();
    past_predictions.push_back(std::make_pair(s->clone(), cur_tree->get_posterior_predictions()));
    
    register_action(action);
    return action;
}

void MCTreePUCTPlayer::register_action(int action){
    if(!cur_tree->expanded)
        cur_tree->expand();
    cur_tree = cur_tree->get_subtree_for_action(action);
}

void MCTreePUCTPlayer::reset(){
    cur_tree = &tree;
    cur_tree->reset();
}

void MCTreePUCTPlayer::update(){
    cur_tree->init_predictor.update(past_predictions);
    cur_tree->reset_probas();
}