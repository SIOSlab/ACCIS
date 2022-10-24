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

        // Previous time
        double t_prev = times.at(k-1);

        // Current time
        t = times.at(k);

        // Previous state estimate
        filter::dist dist_x_prev = states_est.at(k-1);

        // Control torque
        roamps_("TC", tau_c.at(k-1).data());    

        // Predict state
        filter::dist dist_x = filt->predict(t_prev, t,
                    dist_x_prev, dist_w, dyn_filt); 

        // Update state estimate -- GPS
        if (k % cadence_gps == 0)
            dist_x = filt->update(t, z_gps.at(k / cadence_gps), dist_x,
                    gps_err, h_gps);

        // Update state estimate -- star tracker
        if (k % cadence_str == 0)
            dist_x = filt->update(t, z_str.at(k / cadence_str), dist_x,
                    str_err, h_str);
        
        // Update state estimate -- gyroscope
        if (k % cadence_gyr == 0)
            dist_x = filt->update(t, z_gyr.at(k / cadence_gyr), dist_x,
                    gyr_err, h_gyr);

        // Image-based cross-calibration
        if (k == tq.front().step) {

            tq.front().dist_x = dist_x; 

            //dist_x = cc.run(tq.front(), *filt);        

            tq.pop();

        }

        // New state estimate
        states_est.at(k) = dist_x;

    }

}
