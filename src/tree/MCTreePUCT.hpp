#pragma once
#include "State.hpp"
#include "InitPredictor.hpp"
#include <vector>
class MCTreePUCT {
//private:
public:
    State *cur_state;
    int nb_wins;
    int nb_draws;
    int nb_losses;
    int nb_visits;
    
    MCTreePUCT* parent;
    InitPredictor &init_predictor;
    std::vector<float> init_probas;
    
    std::vector<int> allowed_actions;
    std::vector<MCTreePUCT *> subtrees;

    //helper functions for search
    void play_random_and_propagate();
    MCTreePUCT* tree_select();
    
    MCTreePUCT* best_search_child();

    MCTreePUCT(State *s, InitPredictor& init_predictor, MCTreePUCT *parent);
    ~MCTreePUCT(){
        for(auto subtree : subtrees)
            delete subtree;
        delete cur_state;
    };
//public:
    //make simulations to update stats
    void search(int nb_sims);
    
    int select_action();
    ProbasAndValue get_posterior_predictions();

    bool expanded;
    bool children_present;
    bool fresh_probas;
    void expand(); //adds new subtrees

    MCTreePUCT* get_subtree_for_action(int action);
    std::vector<MCTreePUCT *> get_subtrees();

    void reset();
    void reset_probas();

    void DEBUG_print_child_stats();
};