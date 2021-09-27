#include "accis.hpp"

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

    // TO DO
    
    return d;

}
