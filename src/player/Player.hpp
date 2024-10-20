#pragma once
#include "State.hpp"

class Player{
public:
    virtual int play(State *s) = 0;
    virtual void register_action(int action) = 0;
    virtual void reset() = 0;
    virtual void update() = 0;
};