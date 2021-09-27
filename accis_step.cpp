#include "accis.hpp"

void accis_sat::step() {

    // Get current time
    double t = times.back();

    // Get true state
    sat_state x_tru = states_tru.back();

    // Update state estimate -- GPS
    
    // Update state estimate -- Star tracker
    
    // Update state estimate -- Image-based
   
    // Get next time
    double t_next = t + dt;

    // Propagate true state
    sat_state x_tru_next;
    x_tru_next.X = dyn_tru.propagate_random(t, t_next, x_tru.X, rnd);

    // Predict state distribution

}
