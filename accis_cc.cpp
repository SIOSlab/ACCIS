#include "accis.hpp"

#include "roam.h"

#include <queue>

void accis_sat::cross_calibrate() {

    // Current time
    double t = times.back();

    // Get most recent transmissions
    
    std::queue<cross_cal::transmission> tq;
    
    for (const cross_cal::transmission& tr : tr_out)
        if (t - tr.t < cc.dt_max)
            tq.push(tr);

    // Stop if no transmissions
    if (tq.empty()) return;

    // Initial time step
    int k_start = tq.front().step;

    // Re-run filter
    for (int k = k_start; k <= step_no; k++) {

        // Current time
        t = times.at(k);

        // Current state distribution
        filter::dist dist_x = states_est.at(k);

        // Update state estimate -- GPS
        if (step_no % cadence_gps == 0)
            dist_x = filt->update(t, z_gps.at(k), dist_x, gps_err, h_gps);

        // Update state estimate -- star tracker
        if (step_no % cadence_str == 0)
            dist_x = filt->update(t, z_str.at(k), dist_x, str_err, h_str);
        
        // Update state estimate -- gyroscope
        if (step_no % cadence_gyr == 0)
            dist_x = filt->update(t, z_gyr.at(k), dist_x, gyr_err, h_gyr);

        // Image-based cross-calibration
        if (k == tq.front().step) {

            tq.front().dist_x = dist_x; 

            dist_x = cc.run(tq.front(), *filt);        

            tq.pop();

        }

        // New state estimate
        states_est.at(k) = dist_x;

        // Filter prediction
        if (k < step_no) {

            double t_next = t + dt;

            roamps_("TC", tau_c.at(k).data());    
    
            filter::dist dist_x_next = filt->predict(t, t_next,
                    dist_x, dist_w, dyn_tru); 

            states_est.at(k+1) = dist_x_next;

        }

    }

}
