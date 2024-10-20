#pragma once
#include "State.hpp"
#include <random>
#include <vector>

//TODO  : have a general game tree class, which specializes into MCTree, BruteForceTree, etc...
class MCTree {
private:
    inline static std::mt19937 gen = std::mt19937{std::random_device()()};

    constexpr static float C = 1.5f; //TODO : for the moment it is fixed, should prolly add it as a parameter
public:
    State *cur_state;
    int nb_wins;
    int nb_draws;
    int nb_losses;
    int nb_visits;
    
    MCTree* parent;

    std::vector<int> allowed_actions;
    std::vector<MCTree *> subtrees;

    //helper functions for search
    void play_random_and_propagate();
    MCTree* tree_select();
    
    MCTree* best_search_child();

    MCTree(State *s, MCTree *parent);
    ~MCTree(){
        for(auto subtree : subtrees)
            delete subtree;
        delete cur_state;
    };
//public:
    //make simulations to update stats
    void search(int nb_sims);
    int select_action();

    bool expanded;
    void expand(); //adds new subtrees

    MCTree* get_subtree_for_action(int action);
    std::vector<MCTree *> get_subtrees();

    void DEBUG_print_child_stats();
};