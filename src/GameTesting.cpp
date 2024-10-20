#include "Game.hpp"
#include "TicTacToeState.hpp"
#include "BruteForcePlayer.hpp"
#include "MCTreePlayer.hpp"
#include <iostream>

int main(void){
    char tab[3][3]  = {{-1, -1, -1},{-1, -1, -1},{-1, -1, -1}};
    TicTacToeState *s = new TicTacToeState(tab, 1);
    MCTreePlayer p1 = MCTreePlayer(s);
    BruteForcePlayer p2 = BruteForcePlayer(s);
    
    int wins1 = 0, wins2 = 0, draws = 0;
    for(int i = 0; i < 100; ++i){
        int res = play(p1, p2, s);
        if(res == 0) draws++;
        if(res == 1) wins1++;
        if(res == 2) wins2++;
        res = play(p2, p1, s);
        if(res == 0) draws++;
        if(res == 1) wins2++;
        if(res == 2) wins1++;
    }
    delete s;
    std::cout << "W1 D W2 // " << wins1 << " " << draws << " " << wins2 << std::endl;
    return 0;
}