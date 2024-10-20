#include "MCTree.hpp"

#include <cmath>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <ostream>
#include <random>
#include <stdexcept>
#include <vector>
#include <algorithm>

MCTree::MCTree(State *s, MCTree *parent=NULL) : cur_state(s), parent(parent), nb_visits(0), nb_wins(0), nb_draws(0), nb_losses(0), expanded(false){
}

void MCTree::search(int nb_sims){
    for(int i = 0; i<nb_sims; ++i){
        MCTree *leaf_node = this->tree_select();
        leaf_node->play_random_and_propagate();
    }
}

void MCTree::expand(){
    this->expanded = true;
    std::map<int, State*> nexts = this->cur_state->nextActionToStates();
    for (auto const& x : nexts){
        this->allowed_actions.push_back(x.first);
        MCTree *new_subtree = new MCTree(x.second, this);
        this->subtrees.push_back(new_subtree);
    }
}

MCTree *MCTree::best_search_child(){
    auto subtrees = this->subtrees;
    float max_uct = 0.0f;
    MCTree *best_child = NULL;
    for(auto v = subtrees.begin(); v != subtrees.end(); ++v){
        MCTree *cur_child = *v;
        if(cur_child->nb_visits == 0)
            return cur_child;
        else{
            //UCT formula, loss of child node means a win for the current node
            float cur_uct = (1.0*cur_child->nb_losses)/cur_child->nb_visits + C*std::sqrt(std::log(this->nb_visits)/cur_child->nb_visits);
            if(cur_uct > max_uct){
                max_uct = cur_uct;
                best_child = cur_child;
            }
        }
    }
    return best_child;
}

MCTree* MCTree::tree_select(){
    MCTree* nodeptr = this;
    while(true){
        if(!nodeptr->expanded){
            nodeptr->expand();
            return nodeptr;
        }else{
            if(nodeptr->subtrees.size() == 0){
                return nodeptr;
            }else{
                nodeptr = nodeptr->best_search_child();
            }
        }
    }
    throw std::invalid_argument("tree_select() : why on earth did I end up here");
}

void MCTree::play_random_and_propagate(){
    State *init_state = this->cur_state;
    State *s = this->cur_state;
    while(s->eval() == -1){
        auto nextStates = s->nextStates();
        if(nextStates.size() == 0) ;// TODO error case to handle
        else{
            std::uniform_int_distribution<int> dist {0, (int)nextStates.size()-1};
            int index = dist(gen);
            
            if(s != init_state) delete s;
            for(int i = 0; i < nextStates.size(); ++i)
                if(i != index)
                    delete nextStates[i];
            
            s = nextStates[index];
        }
    }
    int result = s->eval();
    if(s != init_state) delete s;

    MCTree *nodeptr = this;
    while(nodeptr != NULL){
        if(result == 0)
            nodeptr->nb_draws += 1;
        else if(result == nodeptr->cur_state->player)
            nodeptr->nb_wins += 1;
        else
            nodeptr->nb_losses += 1;
        nodeptr->nb_visits += 1;
        nodeptr = nodeptr->parent;
    }
}

//TODO max nb visits or fraction of wins?
int MCTree::select_action(){
    auto subtrees = this->subtrees;
    float max_score = -1;
    MCTree *best_child = NULL;
    int best_index = -1;
    for(int i = 0; i < subtrees.size(); ++i){
        MCTree *cur_child = subtrees[i];
        if((cur_child->nb_losses + 0.5*cur_child->nb_draws)/cur_child->nb_visits > max_score){
            max_score = (cur_child->nb_losses + 0.5*cur_child->nb_draws)/cur_child->nb_visits;
            best_child = cur_child;
            best_index = i;
        }
    }
    return this->allowed_actions[best_index];
}

MCTree* MCTree::get_subtree_for_action(int action){
    auto itr = std::find(allowed_actions.begin(), allowed_actions.end(), action);
    if(itr == allowed_actions.cend()){
        throw std::invalid_argument("get_subtree_for_action(): invalid action");
    }
    return subtrees[std::distance(allowed_actions.begin(), itr)];
}

std::vector<MCTree *> MCTree::get_subtrees(){
    return subtrees;
}

void MCTree::DEBUG_print_child_stats(){
    auto actions = this->allowed_actions;
    auto subtrees = this->subtrees;
    std::cout << "State" << std::endl;
    this->cur_state->print();
    std::cout << "WDL " << this->nb_wins << " " << this->nb_draws << " " << this->nb_losses << " visits " << this->nb_visits << std::endl;
    for(int i = 0; i < subtrees.size(); ++i){   
       std::cout << "Action " << actions[i] << "/ WDL " << subtrees[i]->nb_losses << " " << subtrees[i]->nb_draws << " " << subtrees[i]->nb_wins << " visits " << subtrees[i]->nb_visits << std::endl;  
    }
}