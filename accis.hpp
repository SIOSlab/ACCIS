#ifndef ACCIS_SAT_HPP
#define ACCIS_SAT_HPP

#include "filter.hpp"
#include "pydict.hpp"
#include "rando.hpp"
#include "sat_cam.hpp"
#include "sat_meas.hpp"
#include "sat_state.hpp"

#include <memory>
#include <string>
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

        // Satellite ID
        std::string sat_id;
        
        // Current time step
        int step_no;

        // Times
        std::vector<double> times;

        // True states
        std::vector<sat_state> states_tru;

        // State estimates
        std::vector<filter::dist> states_est;

        // Filter
        std::shared_ptr<filter::base> filt;

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

        // Cadences for GPS, star tracker, and imaging
        int cadence_gps;
        int cadence_str;
        int cadence_img;

        // GPS & star tracker measurement models
        sat_meas::gps h_gps;
        sat_meas::star_tracker h_str;

        // GPS & star tracker error distributions
        filter::dist gps_err;
        filter::dist str_err;

};

void show_error(const std::string& msg);

#endif
