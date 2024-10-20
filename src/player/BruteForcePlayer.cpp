#include "BruteForcePlayer.hpp"
#include <cstdlib>

void expand_tree_full(MCTree *tree){
    tree->expand();
    for(auto st : tree->subtrees)
        expand_tree_full(st);
}

void backward_induction(MCTree *tree){
    if(tree->subtrees.size() == 0){
        return;
    }else{
        bool can_win = false, can_draw = false;
        for(auto st : tree->subtrees){
            if(st->subtrees.size() == 0){
                if(st->cur_state->eval() == tree->cur_state->player){
                    can_win = true;
                }else if(st->cur_state->eval() == 0)
                    can_draw = true;
            }else{
                backward_induction(st);
                if(st->nb_draws == 1)
                    can_draw = true;
                if(st->nb_losses == 1)
                    can_win = true;
            }
        }
        if(can_win){
            tree->nb_wins = 1;
        }else if(can_draw){
            tree->nb_draws = 1;
        }else{
            tree->nb_losses = 1;
        }
    }
}

BruteForcePlayer::BruteForcePlayer(State *init_state) 
: tree(MCTree(init_state->clone(), NULL)), cur_tree(&tree){
    expand_tree_full(&tree);
    backward_induction(&tree);
}

int BruteForcePlayer::play(State *s){
    //assert(s == cur_tree->cur_state);
    int draw_action = -1;

    for(int i = 0; i<cur_tree->allowed_actions.size(); ++i){
        int action = cur_tree->allowed_actions[i];
        MCTree* subtree = cur_tree->subtrees[i];
        if(subtree->cur_state->eval() != -1){
            int eval = subtree->cur_state->eval();
            if(eval == 0){
                draw_action = action;
            }else if(eval == cur_tree->cur_state->player){
                register_action(action);
                return action;
            }
        }else{
            if(subtree->nb_losses == 1){
                register_action(action);
                return action;
            }else if(subtree->nb_draws == 1){
                draw_action = action;
            }
        }
    }
    
    if(draw_action != -1){
        register_action(draw_action);
        return draw_action;
    }else{
        int action = cur_tree->allowed_actions[0];
        register_action(action);
        return action;
    }
}
void BruteForcePlayer::register_action(int action){
    cur_tree = cur_tree->get_subtree_for_action(action);
}
void BruteForcePlayer::reset(){
    cur_tree = &tree;
}

int nb_states(MCTree *t){
    int sum_children_states = 0;
    for (auto st : t->subtrees){
        sum_children_states += nb_states(st);
    }
    return 1 + sum_children_states;
}

int BruteForcePlayer::DEBUG_nb_states(){
    return nb_states(&tree);
}