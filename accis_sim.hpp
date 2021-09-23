#ifndef ACCIS_SIM_HPP
#define ACCIS_SIM_HPP

#include "accis_io.hpp"

#include <vector>

std::vector<sat_results> run_accis_sim(const sim_info& sim_i,
        const std::vector<sat_info>& sat_i);

#endif
