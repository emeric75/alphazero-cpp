#pragma once
#include "State.hpp"
#include <array>

using std::array;

class TicTacToeState : public State{
private:
  //board contains -1(EMPTY), 1 or 2 (player 1 or 2 has played here)
  array<char, 9> board;
  //player is 1 or 2 (player that has to play at the given state)
  //int player;
public:
  TicTacToeState(char board[3][3], int player);
  TicTacToeState(array<char, 9> board, int player);
  ~TicTacToeState() override {};

  static const int nb_players = 2;
  static const int nb_actions = 9;

  int get_nb_actions() override;
  bool isActionValid(int action) override;
  State* nextState(int action) override;
  
  std::map<int, State*> nextActionToStates() override;
  
  TicTacToeState *clone () override{
    return new TicTacToeState(*this);
  }
  
  array<char, 9> getBoard(){
    return board;
  }

  char eval() override;

  void print() override;
};
