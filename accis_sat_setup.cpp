#include "accis_sat.hpp"

#include "house.hpp"
#include "ukf.hpp"

accis_sat::accis_sat(const sim_info& sim_i, const sat_info& sat_i) {

    // Initialize filter
    if (sat_i.filter_type == "UKF")
        filt.reset(new ukf());
    else if (sat_i.filter_type == "HOUSE")
        filt.reset(new house());

}
