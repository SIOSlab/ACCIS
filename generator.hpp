#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "sat_state.hpp"

#include <string>

class generator {

    public:

        void run();

    private:

        int seed;
            
        double max_alt;
        double min_alt;

        sat_state_randomizer rzer;

};

#endif
