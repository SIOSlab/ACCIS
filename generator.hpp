#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "pydict.hpp"
#include "sat_cam.hpp"
#include "sat_state.hpp"

#include <string>

class generator {

    public:

        generator();

        void set_param(const pydict::dict& d);

        pydict::dict get_param();
        
        void run();

        mat<> get_diffs();

        sat_state gen_state(rando& rnd);

    private:

        // Called by constructor and set_param
        void setup();

        // Input parameters
        pydict::dict par;

        int seed;

        int max_imgs;

        double max_blp;

        double avg_alt;
        double var_alt;

        sat_cam cam;
        
        sat_state_randomizer rzer;

        int num_pts;

        double max_dist;

};

#endif
