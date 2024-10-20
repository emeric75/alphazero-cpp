#include "TicTacToeState.hpp"
#include "MCTree.hpp"
#include <cstdlib>
#include <chrono>
#include <iostream>
int main(void){
    char tab[3][3]  = {{-1, -1, -1},{-1, -1, -1},{-1, -1, -1}};
    int nb_sims[] =   {1000, 
                       2000, 
                       4000,
                       8000, 
                       16000,  
                       32000,
                       64000,
                       128000,
                       256000,
                       512000,
                       1024000};
    std::cout << "NB SIMS\tTIME(ms)" << std::endl;
    for(int nb : nb_sims){
      State *s = new TicTacToeState(tab, 1);
      MCTree mc = MCTree(s, NULL);
      auto start = std::chrono::steady_clock::now();
      mc.search(nb);
      auto end = std::chrono::steady_clock::now();
      auto diff = end-start;

      std::cout << nb << "\t" << std::chrono::duration <double, std::milli> (diff).count() << std::endl;
    }

}
