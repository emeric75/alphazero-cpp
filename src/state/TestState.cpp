#include "TestState.hpp"
#include <cstdio>

TestState::TestState(int a, int player) : a(a), player(player)
{
}

bool TestState::isActionValid(int action){
  return true;
}

State* TestState::nextState(int action){
  return this;
}

std::map<int, State*> TestState::nextActionToStates(){
  std::map<int, State*> out;
  return out;
}

char TestState::eval(){
  return -1;
}

void TestState::print(){
  printf("TestState : nbPlayers %d\n", this->nb_players);
}
