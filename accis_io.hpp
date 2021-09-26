#ifndef ACCIS_IO_HPP
#define ACCIS_IO_HPP

#include <mat.hpp>

#include <string>
#include <vector>

struct sim_info {
    int num_steps = 100;
};

struct sat_info {
    std::string filter_type = "UKF";
};

struct sat_results {
    int num_steps = 100;
};

#endif
