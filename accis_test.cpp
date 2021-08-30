#include "eigen_csv.hpp"
#include "filter.hpp"
#include "house.hpp"
#include "mat.hpp"
#include "nlohmann/json.hpp"
#include "sat_cam.hpp"
#include "sat_state.hpp"
#include "ukf.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Basic information about simulation
struct sim_info {
    std::vector<std::string> filters;
    std::string orbit_file;
    std::string camera_file;
    int num_trials;
    int num_steps;
    double step_size;
    int gps_cadence;
    int star_tracker_cadence;
    int image_cadence;
};

// Set up JSON readers
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(sim_info, filters, orbit_file, camera_file,
        num_trials, num_steps, step_size, gps_cadence, star_tracker_cadence,
        image_cadence)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(sat_cam, widp, lenp, rho, u, A) 


// Main function
int main(int argc, char** argv) {

    // Get basic simulation info
    std::string si_file(argv[1]);
    nlohmann::json si_json(si_file);
    sim_info si = si_json.get<sim_info>();

    // Get nominal satellite orbits
    mat<> orbit_table;
    eigen_csv::read(si.orbit_file, true, true, orbit_table);
    int num_sats = orbit_table.rows();
    std::vector<coe> orbit(num_sats);
    for (int i = 0; i < num_sats; i++)
        orbit[i].Y = orbit_table.row(i);

    // Satellite camera model
    nlohmann::json cam_json(si.camera_file);
    sat_cam camera = cam_json.get<sat_cam>();

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

    // Iterate over filter types
    for (int filter_no = 0; filter_no < filter_list.size; filter_no++) {

        // Iterate over trials
        for (int trial = 0; trial < si.num_trials; trial++) {

            std::cout << "-----------------------------------" << std::endl;
            std::cout << "Filter: " << si.filters[filter_no]   << std::endl;
            std::cout << "Trail:  " << trial                   << std::endl; 
            std::cout << "-----------------------------------" << std::endl;

            // TO DO

        }

    }

}
