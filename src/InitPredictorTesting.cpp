#include "TicTacToeInitPredictor.hpp"
#include <torch/torch.h>
int main() {
  TicTacToeInitPredictor ip;
  char tab[3][3]  = {{-1, -1, -1},{-1, -1, -1},{-1, -1, -1}};
  State *s = new TicTacToeState(tab, 1);
  for(int i = 0; i<50; ++i)
    ip.get(*s).print();
  delete s;
  return 0;
}