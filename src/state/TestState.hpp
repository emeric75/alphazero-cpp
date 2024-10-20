#include "State.hpp"
#include <map>

class TestState : public State{
private:
  int a;
public:
  TestState(int a, int player);
  ~TestState() override {};

  static const int nb_players = 42;
  static const int nb_actions = 9;
  int player;

  int get_nb_actions() override { return 0; }
  bool isActionValid(int action) override;
  State* nextState(int action) override;
  std::map<int, State*> nextActionToStates() override;
  char eval() override;

  TestState *clone () override{
    return new TestState(*this);
  }
  
  void print() override;
};
