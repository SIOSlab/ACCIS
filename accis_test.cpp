
#include "eigen_csv.hpp"
#include "mat.hpp"
#include "sat_cam.hpp"
#include "sat_state.hpp"

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {

    using namespace Eigen;

    // Get nominal satellite orbits
    mat<> orbit_table;
    eigen_csv::read("input/orbits.csv", true, true, orbit_table);
    int num_sats = orbit_table.rows();
    std::vector<coe> orbit(num_sats);
    for (int i = 0; i < num_sats; i++)
        orbit[i].Y = orbit_table.row(i);

    // Satellite camera model
    sat_cam camera; // TO DO 

    // Get nominal initial states
    std::vector<sat_state> init_ideal_state(num_sats);
    for (int i = 0; i < num_sats; i++) {
        init_ideal_state[i].set_coe(orbit[i]);
        init_ideal_state[i].set_ideal_cam(camera.u);
        init_ideal_state[i].set_nadir();
    }    

}
