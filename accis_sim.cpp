#include "accis_sim.hpp"

#include "roam.h"

void run_accis_sim(const time_info& t_info, filter::base& filt,
        const std::string& filt_name, int trial_no,
        const std::vector<sat_state>& init_ideal_state,
        const sat_cam& camera) {

    // Random number generator
    rando rnd(trial_no);

    // Set epoch
    roamps_("T0", &t_info.start_time);

    // Iterate over time steps
    for (int k = 0; k <= t_info.num_steps; k++) {

        // Propagate satellite states

        // Filter prediction

        // GPS Update
        
        // Star Tracker Update

        // Acquire image

        // Image-based update

    }

    // Save results

}
