#pragma once
#include "Chips.hpp"
class Machine {
public:
    Machine();

    void reset();
    void step();
    void run();
};
