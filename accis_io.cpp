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

    mat<> x_tru(sat_state::N, step_no), x_est(sat_state::N, step_no);
    vec<> t(step_no);
    mat<> x_err(sat_state_err::N, step_no); 
    for (int k = 0; k < step_no; k++) {
        x_tru.col(k) = states_tru[k].X;
        x_est.col(k) = states_est[k].mean;
        t(k) = times[k];
        x_err.col(k) = sat_state_err::diff(states_tru[k].X,
                states_est[k].mean).dX;
    }
   
    pydict::set<mat<>>(d, "True States", x_tru);
    pydict::set<mat<>>(d, "Estimated States", x_est);
    pydict::set<vec<>>(d, "Time (s)", t);
    pydict::set<mat<>>(d, "State Error", x_err);

    return d;

}

void show_error(const std::string& msg) {
    std::cout << msg << std::endl;
    exit(EXIT_FAILURE);
}