#include "accis_sim.hpp"

#include "roam.h"

void run_accis_sim(const time_info& t_info, filter::base& filt,
        const std::string& filt_name, int trial_no,
        const std::vector<sat_state>& init_ideal_state,
        sat_cam& camera, sat_state_randomizer& rzer,
        const std::string& results_dir) {

    // Number of satellites
    int num_sat = init_ideal_state.size();

    // Random number generator
    rando rnd(trial_no);

    // Set epoch
    roamps_("T0", &t_info.start_time); 

    // Initialize true state
    std::vector<std::vector<sat_state>> state_tru(num_sat);
    for (int i = 0; i < num_sat; i++)
        state_tru[i].push_back(rzer.randomize(init_ideal_state[i], rnd));

    // Initialize state estimate distributions
    filter::dist dist_x0(sat_state::N);
    dist_x0.cov = rzer.cov();
    vec<> state_skew(sat_state::N), state_kurt(sat_state::N);
    state_skew.setZero();
    state_kurt.setConstant(3);
    dist_x0.par.push_back(state_skew);
    dist_x0.par.push_back(state_kurt);
    std::vector<std::vector<filter::dist>> state_est(num_sat);
    for (int i = 0; i < num_sat; i++) {
        dist_x0.mean = init_ideal_state[i].X;
        state_est[i].push_back(dist_x0);
    }

    // Times
    vec<> times;
    times.setLinSpaced(t_info.num_steps+1, 0,
            t_info.num_steps * t_info.step_size);

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
    for (int i = 0; i < num_sat; i++) {
        std::string results_file = "results/" + results_dir + "sat_"
            + std::to_string(i+1) + "_" + filt_name + "_"
            + std::to_string(trial_no);
        sat_state_save(results_file, times, state_tru[i], state_est[i]);  
    }

}
