#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "sat_cam.hpp"
#include "sat_state.hpp"

#include <string>

class generator {

    public:

        void run();

    private:

        int seed;

        int max_pairs;

        int num_pts;

        double dmax;

        double max_blp;

        double max_alt;
        double min_alt;

        sat_cam cam;
        
        sat_state_randomizer rzer;

};

#endif
