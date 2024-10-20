#include "TicTacToeState.hpp"
#include <cstdio>

#include <iostream>
#include <ostream>

#include <stdexcept>

TicTacToeState::TicTacToeState(char board[3][3], int player){
  for(int i = 0; i<3; ++i){
    for(int j = 0; j<3; ++j){
      this->board[3*i+j] = board[i][j];
    }
  }

  this->player = player;
}

TicTacToeState::TicTacToeState(array<char, 9> board, int player){
  for(int i = 0; i<9; ++i){
    this->board[i] = board[i];
  }

  this->player = player;
}

int TicTacToeState::get_nb_actions(){
  return 9;
}

bool TicTacToeState::isActionValid(int action){
  return this->eval() == -1 && this->board[action] == -1;
}

State* TicTacToeState::nextState(int action){
  if(this->board[action] == -1){
    TicTacToeState *newState = new TicTacToeState(*this);
    newState->board[action] = this->player;
    switch(this->player){
      case 1: newState->player = 2; break;
      case 2: newState->player = 1; break;
      default: printf("%s\n", "Invalid token in TicTacToeState"); //oops!
    }
    return newState;
  }
  else{
    throw std::invalid_argument("TicTacToeState::nextState : invalid action");
  }
}

std::map<int, State*> TicTacToeState::nextActionToStates(){
  std::map<int, State*> outs;
  if(eval() == -1){ //if game is finished, no more action possible
    for(int i = 0; i<9; ++i){
      if(this->board[i] == -1){
        TicTacToeState *newState = new TicTacToeState(*this);
        newState->board[i] = this->player;
        switch(this->player){
          case 1: newState->player = 2; break;
          case 2: newState->player = 1; break;
          default: printf("%s\n", "Invalid token in TicTacToeState"); //oops!
        }
        outs[i] = newState;
      }
    }
  }

  return outs;
}


int wins[8][3] = {
  {0,3,6},
  {1,4,7},
  {2,5,8},
  {0,1,2},
  {3,4,5},
  {6,7,8},
  {0,4,8},
  {6,4,2}
};
char TicTacToeState::eval(){
  for(int i = 0; i<8; ++i){
    if(this->board[wins[i][0]] == this->board[wins[i][1]] && this->board[wins[i][1]] == this->board[wins[i][2]] && this->board[wins[i][2]] != -1){
      return this->board[wins[i][0]];
    }
  }

  char all_filled = 1;
  for(int i = 0; i<9; ++i){
    if(this->board[i] == -1)
      all_filled = 0;
  }

  if (all_filled)
    return 0;
  else
    return -1;
}

void TicTacToeState::print(){
  std::cout << "TicTacToeState" << std::endl;
  for(int i = 0; i<3; ++i){
    printf("%2d %2d %2d\n", this->board[3*i], this->board[3*i+1], this->board[3*i+2]);
  }
  std::cout << "Player to play : " << this->player << std::endl;
}
