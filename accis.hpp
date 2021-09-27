#ifndef ACCIS_SAT_HPP
#define ACCIS_SAT_HPP

#include "filter.hpp"
#include "pydict.hpp"
#include "rando.hpp"
#include "sat_cam.hpp"
#include "sat_state.hpp"

#include <vector>

class accis_sat {

    public:

        accis_sat();

        void set_param(const pydict::dict& d);

        pydict::dict get_param();

        pydict::dict get_results();

        void step();

        void transmit(accis_sat& target);

    private:

        // Called by constructor and set_param
        void setup();

        // Input parameters
        pydict::dict par;

        // Times
        std::vector<double> times;

        // True states
        std::vector<sat_state> states_tru;

        // State estimates
        std::vector<filter::dist> states_est;

        // Camera model
        sat_cam cam;

        // Satellite dynamic model (ground truth propagation & filtering)
        sat_dyn dyn_tru;
        sat_dyn dyn_filt;

        // Distribution of satellite process noise for filtering
        filter::dist dist_w;

        // Time step
        double dt;

        // Random number generator
        rando rnd;

};

void accis_run(std::vector<accis_sat>& sats, int steps);

#endif
