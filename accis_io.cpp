#include "accis.hpp"

#include <iostream>

accis_sat::accis_sat() {
    setup();
}

void accis_sat::set_param(const pydict::dict& d) {
    par = d;
    setup();
}

pydict::dict accis_sat::get_param() {
    return par;
}

pydict::dict accis_sat::get_results() {

    pydict::dict d;

    mat<> x_tru(step_no, sat_state::N), x_est(step_no, sat_state::N);
    vec<> t(step_no);
    for (int k = 0; k < step_no; k++) {
        x_tru.row(k) = states_tru[k].X;
        x_est.row(k) = states_est[k].mean;
        t(k) = times[k];
    }
   
    pydict::set<mat<>>(d, "True States", x_tru);
    pydict::set<mat<>>(d, "Estimated States", x_est);
    pydict::set<vec<>>(d, "Time (s)", t);

    return d;

}

void show_error(const std::string& msg) {
    std::cout << msg << std::endl;
    exit(EXIT_FAILURE);
}
