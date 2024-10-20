#include "MCTreePUCT.hpp"

#include <cmath>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <random>
#include <stdexcept>
#include <vector>
#include <algorithm>

std::random_device rd;  // a seed source for the random number engine
std::mt19937 gen(rd());

float C = 1.5f; //TODO : for the moment it is fixed, should prolly add it as a parameter

MCTreePUCT::MCTreePUCT(State *s, InitPredictor& init_predictor, MCTreePUCT *parent=NULL) 
: cur_state(s), 
  parent(parent), 
  nb_visits(0), 
  nb_wins(0), 
  nb_draws(0), 
  nb_losses(0), 
  expanded(false), 
  children_present(false),
  fresh_probas(true),
  init_predictor(init_predictor), 
  init_probas(init_predictor.get(*s).probas)
{
}

void MCTreePUCT::search(int nb_sims){
    for(int i = 0; i<nb_sims; ++i){
        MCTreePUCT *leaf_node = this->tree_select();
        leaf_node->play_random_and_propagate();
    }
}

void MCTreePUCT::expand(){
    this->expanded = true;
    if(!children_present){
        std::map<int, State*> nexts = this->cur_state->nextActionToStates();
        for (auto const& x : nexts){
            this->allowed_actions.push_back(x.first);
            MCTreePUCT *new_subtree = new MCTreePUCT(x.second, this->init_predictor, this);
            this->subtrees.push_back(new_subtree);
        }
        children_present = true;
    }
}

MCTreePUCT *MCTreePUCT::best_search_child(){
    auto subtrees = this->subtrees;
    float max_puct = 0.0f;
    MCTreePUCT *best_child = NULL;
    if(!fresh_probas){
        init_probas = init_predictor.get(*cur_state).probas;
        fresh_probas = true;
    }

    for(int i = 0; i<allowed_actions.size(); ++i){
        int cur_action = allowed_actions[i];
        MCTreePUCT *cur_child = subtrees[i];
        if(cur_child->nb_visits == 0)
            return cur_child;
        else{
            //PUCT formula, loss of child node means a win for the current node
            float cur_puct = (1.0*cur_child->nb_losses)/cur_child->nb_visits + C*init_probas[allowed_actions[i]]*std::sqrt(this->nb_visits)/(1+cur_child->nb_visits);
            if(cur_puct > max_puct){
                max_puct = cur_puct;
                best_child = cur_child;
            }
        }
    }
    return best_child;
}

MCTreePUCT* MCTreePUCT::tree_select(){
    MCTreePUCT* nodeptr = this;
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

void MCTreePUCT::play_random_and_propagate(){
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

    MCTreePUCT *nodeptr = this;
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

//TODO random play instead of greedy play?
//see get_posterior_predictions
int MCTreePUCT::select_action(){
    auto subtrees = this->subtrees;
    float max_score = -1;
    MCTreePUCT *best_child = NULL;
    int best_index = -1;
    for(int i = 0; i < subtrees.size(); ++i){
        MCTreePUCT *cur_child = subtrees[i];
        if((cur_child->nb_losses + 0.5*cur_child->nb_draws)/cur_child->nb_visits > max_score){
            max_score = (cur_child->nb_losses + 0.5*cur_child->nb_draws)/cur_child->nb_visits;
            best_child = cur_child;
            best_index = i;
        }
    }
    return this->allowed_actions[best_index];
}

//TODO add temperature parameter
ProbasAndValue MCTreePUCT::get_posterior_predictions(){
    std::vector<float> probas(cur_state->get_nb_actions(),0.0);
    float value;
    for(int i = 0; i<allowed_actions.size(); ++i){
        int cur_action = allowed_actions[i];
        MCTreePUCT *cur_child = subtrees[i];
        probas[allowed_actions[i]] = 1.0*cur_child->nb_visits/this->nb_visits;
    }
    value = 1.0*(nb_wins-nb_losses)/nb_visits;
    return ProbasAndValue{.probas=probas, .value=value};
}

MCTreePUCT* MCTreePUCT::get_subtree_for_action(int action){
    auto itr = std::find(allowed_actions.begin(), allowed_actions.end(), action);
    if(itr == allowed_actions.cend()){
        throw std::invalid_argument("get_subtree_for_action(): invalid action");
    }
    return subtrees[std::distance(allowed_actions.begin(), itr)];
}

std::vector<MCTreePUCT *> MCTreePUCT::get_subtrees(){
    return subtrees;
}

void MCTreePUCT::reset(){
    for(auto st : subtrees)
        st->reset();
    nb_visits = 0; 
    nb_wins = 0;
    nb_draws = 0; 
    nb_losses = 0; 
    expanded = false;
}

void MCTreePUCT::reset_probas(){
    for(auto st : subtrees)
        st->reset_probas();
    fresh_probas = false;
}

void MCTreePUCT::DEBUG_print_child_stats(){
    auto actions = this->allowed_actions;
    auto subtrees = this->subtrees;
    std::cout << "State" << std::endl;
    this->cur_state->print();
    std::cout << "WDL " << this->nb_wins << " " << this->nb_draws << " " << this->nb_losses << " visits " << this->nb_visits << std::endl;
    for(int i = 0; i < subtrees.size(); ++i){   
       std::cout << "Action " << actions[i] << "/ WDL " << subtrees[i]->nb_losses << " " << subtrees[i]->nb_draws << " " << subtrees[i]->nb_wins << " visits " << subtrees[i]->nb_visits << std::endl;  
    }
}