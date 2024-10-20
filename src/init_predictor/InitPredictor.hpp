#pragma once
#include "State.hpp"
#include <iostream>
#include <vector>

struct ProbasAndValue {
    std::vector<float> probas;
    float value;
    void print(){
        std::cout << "Probas" << std::endl;
        for(auto it = probas.begin(); it != probas.end(); ++it)
            std::cout << (*it) << " ";
        std::cout << std::endl;
        std::cout << "Value " << value << std::endl;
    }
};

class InitPredictor{
private:
    static int nb_actions;
public:
    virtual ProbasAndValue get(State &s) = 0;
    virtual void update(std::vector<std::pair<State*, ProbasAndValue>> results) = 0;
};