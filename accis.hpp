#ifndef ACCIS_SAT_HPP
#define ACCIS_SAT_HPP

#include "attitude_pd.hpp"
#include "cross_cal.hpp"
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

        //----------------------------------------------------------------------

        // Called by constructor and set_param
        void setup();

        // Input parameters
        pydict::dict par;

        // Satellite ID
        int sat_id;
        
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

        // Cadences for measurements and imaging
        int cadence_gps;
        int cadence_str;
        int cadence_gyr;
        int cadence_img;

        // Measurement models
        sat_meas::gps h_gps;
        sat_meas::star_tracker h_str;
        sat_meas::gyro h_gyr;

        // Measurement error distributions
        filter::dist gps_err;
        filter::dist str_err;
        filter::dist gyr_err;

        // Attitude control algorithm
        attitude_pd att_ctrl;

        // Principal moments of inertia
        vec<3> J;

        // Cross-calibrator
        cross_cal cc;

        // Latest transmission
        cross_cal::transmission tr_last;

        // Maximum allowed percentage of black pixels
        double max_blp;

        // Attitude state errors
        std::vector<vec<2>> att_err;

};

void show_error(const std::string& msg);

#endif
