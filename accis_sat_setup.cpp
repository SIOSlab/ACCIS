#include "accis_sat.hpp"

accis_sat::accis_sat(const std::unordered_map<std::string,int>& par_i_in,
                     const std::unordered_map<std::string,double>& par_d_in) :
        params_i(par_i_in),
        params_d(par_d_in) {

    // TO DO
        
}

std::unordered_map<std::string,int> accis_sat::get_params_i() {
    return params_i;
}

std::unordered_map<std::string,double> accis_sat::get_params_d() {
    return params_d;
}

int accis_sat::get_pard_i(const std::string var, int val) {
    if (params_i.count(var) == 0)
        params_i[var] = val;
    return params_i[var];
}


double accis_sat::get_pard_d(const std::string var, double val) {
    if (params_d.count(var) == 0)
        params_d[var] = val;
    return params_d[var];
}
