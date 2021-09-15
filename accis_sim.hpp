#ifndef ACCIS_SIM_HPP
#define ACCIS_SIM_HPP

#include "filter.hpp"
#include "sat_cam.hpp"
#include "sat_state.hpp"

#include <string>
#include <vector>

struct time_info {
    int num_steps;
    double step_size;
    double start_time;
    int gps_cadence;
    int star_tracker_cadence;
    int image_cadence;
};

void run_accis_sim(const time_info& t_info, filter::base& filt,
        const std::string& filt_name, int trial_no,
        const std::vector<sat_state>& init_ideal_state,
        const sat_cam& camera);


#endif
