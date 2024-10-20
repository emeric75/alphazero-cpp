#ifndef STATE_HPP
#define STATE_HPP

#include <map>
#include <vector>

class State{
public:
  //there are nb_players in the game, named 1 2 ... nb_players
  static int nb_players;
  //the maximum number of actions possible in the game
  //static int nb_actions;

  //the player that has to play at this state
  int player;

  virtual ~State() {};

  //returns the number of actions possible in the game
  //it should not depend on the current state
  virtual int get_nb_actions() = 0;

  virtual bool isActionValid(int action) = 0;
  virtual State* nextState(int action) = 0;
  
  virtual std::map<int, State*> nextActionToStates() = 0;
  std::vector<State*> nextStates(){
    std::vector<State *> outs;
    for(auto p : this->nextActionToStates()){
      outs.push_back(std::get<1>(p));
    }
    return outs;
  };
  std::vector<int> allowedActions(){
    std::vector<int> outs;
    for(auto p : this->nextActionToStates()){
      outs.push_back(std::get<0>(p));
    }
    return outs;
  };

  //Return -1 if game is not finished
  //Return 0 for DRAW
  //Return id of winning player if not a draw
  //If game has no actions left, eval() cannot return -1
  virtual char eval() = 0;

  //return pointer to copy of state
  virtual State * clone() = 0;
  
  //debugging purposes
  virtual void print() = 0;
};

#endif /* end of include guard: STATE_HPP */
