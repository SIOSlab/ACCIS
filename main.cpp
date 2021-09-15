#include "accis_sim.hpp"
#include "eigen_csv.hpp"
#include "ez_json.hpp"
#include "filter.hpp"
#include "house.hpp"
#include "mat.hpp"
#include "sat_cam.hpp"
#include "sat_state.hpp"
#include "ukf.hpp"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Basic information about simulation
struct sim_info {
    std::vector<std::string> filters;
    std::string orbit_file;
    std::string camera_file;
    std::string time_file;
    std::string err_file;
    int num_trials;
};

// Set up JSON readers
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(sim_info, filters, orbit_file, camera_file,
        time_file, err_file, num_trials)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(time_info, num_steps, step_size, start_time,
        gps_cadence, star_tracker_cadence, image_cadence)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(sat_cam, widp, lenp, rho, u, A)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(sat_state_randomizer, stdr, stdv, stdw,
        stdba, stdca, stdf, stdc)

// Main function
int main(int argc, char** argv) {

    // Get basic simulation info
    std::string si_file = (argc == 1) ? "input/setup.json" : argv[1];
    sim_info si;
    ez_json_read(si_file, si);

    // Get nominal satellite orbits
    mat<> orbit_table;
    eigen_csv::read(si.orbit_file, true, true, orbit_table);
    int num_sats = orbit_table.rows();
    std::vector<coe> orbit(num_sats);
    for (int i = 0; i < num_sats; i++)
        orbit[i].Y = orbit_table.row(i);

    // Satellite camera model
    sat_cam camera;
    ez_json_read(si.camera_file, camera);

    // Time information
    time_info t_info;
    ez_json_read(si.time_file, t_info);

    // State randomizer
    sat_state_randomizer rzer;
    ez_json_read(si.err_file, rzer);

    // Get nominal initial states
    std::vector<sat_state> init_ideal_state(num_sats);
    for (int i = 0; i < num_sats; i++) {
        init_ideal_state[i].set_coe(orbit[i]);
        init_ideal_state[i].set_ideal_cam(camera.u);
        init_ideal_state[i].set_nadir();
    }    

    // Create filters
    std::vector<std::shared_ptr<filter::base>> filter_list;
    for (const std::string& filt_name : si.filters) {
        if (filt_name == "HOUSE") {
            filter_list.emplace_back(new house());
        } else if (filt_name == "UKF") {
            filter_list.emplace_back(new ukf());
        } else {
            std::cout << "Unkown filter type!" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    int num_filters = filter_list.size();

    // Iterate over filter types
    for (int filt_no = 0; filt_no < num_filters; filt_no++) {

        // Iterate over trials
        for (int trial_no = 0; trial_no < si.num_trials; trial_no++) {

            std::cout << "-----------------------------------" << std::endl;
            std::cout << "Filter: " << si.filters[filt_no]     << std::endl;
            std::cout << "Trail:  " << trial_no                << std::endl; 
            std::cout << "-----------------------------------" << std::endl;

            run_accis_sim(t_info, *filter_list[filt_no], si.filters[filt_no],
                trial_no, init_ideal_state, camera, rzer); 

        }

    }

}
