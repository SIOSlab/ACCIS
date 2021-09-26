#include "accis_sat.hpp"

#include "house.hpp"
#include "ukf.hpp"

#include <iostream>

accis_sat::accis_sat(const sim_info& sim_i, const sat_info& sat_i) {

    // Initialize filter
    if (sat_i.filter_type == "UKF")
        filt.reset(new ukf());
    else if (sat_i.filter_type == "HOUSE")
        filt.reset(new house());
    else
        show_error("Filter type not recognized");



}

sat_results accis_sat::get_results() {
    sat_results res;
    // TO DO!!
    return res;
}

void accis_sat::show_error(const std::string& msg) {
    std::cout << msg << std::endl;
    exit(EXIT_FAILURE);
}
