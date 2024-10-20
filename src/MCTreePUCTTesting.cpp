#include "MCTreePUCTPlayer.hpp"
#include "TicTacToeState.hpp"
#include "TicTacToeInitPredictor.hpp"
#include "BruteForcePlayer.hpp"
#include "Game.hpp"
#include <iostream>

int main(void){
    char tab[3][3]  = {{-1, -1, -1},{-1, -1, -1},{-1, -1, -1}};
    State *s = new TicTacToeState(tab, 1);
    TicTacToeInitPredictor pred = TicTacToeInitPredictor();
    MCTreePUCTPlayer treeplayer = MCTreePUCTPlayer(s, pred);
    BruteForcePlayer bfplayer = BruteForcePlayer(s);
    std::cout << "Init prediction" << std::endl;
    pred.get(*s).print();
    int nb_ep = 10;
    int nb_rounds_per_ep = 200;
    for(int ep = 0; ep < nb_ep; ++ep){
        std::cout << "Ep " << ep << std::endl;
        auto game_start = std::chrono::steady_clock::now();
        for(int i = 0; i<nb_rounds_per_ep; ++i){
            play(treeplayer, bfplayer, s);
            treeplayer.reset();
            play(bfplayer, treeplayer, s);
            treeplayer.reset();
        }
        auto train_start = std::chrono::steady_clock::now();
        treeplayer.update();
        auto train_end = std::chrono::steady_clock::now();
        std::cout << "Game time : " << std::chrono::duration <double> (train_start-game_start).count() << " sec" << std::endl;
        std::cout << "Train time : " << std::chrono::duration <double> (train_end-train_start).count() << " sec" << std::endl;
        std::cout << "New prediction " << ep << std::endl;
        pred.get(*s).print();
    }
    return 0;
}