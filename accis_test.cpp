#include "eigen_csv.hpp"
#include "filter.hpp"
#include "house.hpp"
#include "mat.hpp"
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

int main(int argc, char** argv) {

    // Get basic simulation info
    sim_info si;
    // TO DO: Get values

    // Get nominal satellite orbits
    mat<> orbit_table;
    eigen_csv::read(si.orbit_file, true, true, orbit_table);
    int num_sats = orbit_table.rows();
    std::vector<coe> orbit(num_sats);
    for (int i = 0; i < num_sats; i++)
        orbit[i].Y = orbit_table.row(i);

    // Satellite camera model
    sat_cam camera; // TO DO: Get parameters

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
        if (filt_name == "HOUSE")
            filter_list.emplace_back(new house());
        else if (filt_name == "UKF")
            filter_list.emplace_back(new ukf());
    }

}
