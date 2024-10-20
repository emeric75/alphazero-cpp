#include "BruteForcePlayer.hpp"
#include "TicTacToeState.hpp"
#include <iostream>

int main(void){
    char tab[3][3]  = {{-1, -1, -1},{-1, -1, -1},{-1, -1, -1}};
    State *s = new TicTacToeState(tab, 1);
    BruteForcePlayer p1 = BruteForcePlayer(s);
    std::cout << p1.DEBUG_nb_states() << std::endl;
    return 0;
}