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
        for (int trial = 0; trial < si.num_trials; trial++) {

            std::cout << "-----------------------------------" << std::endl;
            std::cout << "Filter: " << si.filters[filt_no]   << std::endl;
            std::cout << "Trail:  " << trial                   << std::endl; 
            std::cout << "-----------------------------------" << std::endl;

            // Iterate over time steps
            for (int k = 0; k < si.num_steps; k++) {

                // Update ground truth state for each satellite
                if (k > 0) {

                    for (int sat_no = 0; sat_no < num_sats; sat_no++) {

                        
                         

                    }

                }

            }

        }

    }

}
