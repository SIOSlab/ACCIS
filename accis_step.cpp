#include "accis.hpp"

#include "angles.hpp"
#include "format.hpp"
#include "roam.h"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp> 
#include <opencv2/imgproc.hpp>

#include <string>

void accis_sat::step() {

    // Get current time
    double t = times.back();

    // Get true state
    sat_state x_tru = states_tru.back();

    // Get current state estimate distribution
    filter::dist dist_x = states_est.back();

    // Update state estimate -- GPS
    if (step_no % cadence_gps == 0) {

        vec<> z = h_gps.H(t, x_tru.X, rnd);

        dist_x = filt->update(t, z, dist_x, gps_err, h_gps);

    }

    // Update state estimate -- Star tracker
    if (step_no % cadence_str == 0) {

        vec<> z = h_str.H(t, x_tru.X, rnd);

        dist_x = filt->update(t, z, dist_x, str_err, h_str);

    }
    
    // Update state estimate -- Gyroscope
    if (step_no % cadence_gyr == 0) {

        vec<> z = h_gyr.H(t, x_tru.X, rnd);

        dist_x = filt->update(t, z, dist_x, str_err, h_gyr);

    }

    // Imaging & cross-calibration
    if (cadence_img != 0 && step_no % cadence_img == 0) {

        // Get image
        cv::Mat image = cam.real_image(t, x_tru);

        // Percentage of black pixels
        cv::Mat gray;
        cv::cvtColor(image, gray, cv::COLOR_RGB2GRAY);
        double blp = 100.0 - (100.0 * cv::countNonZero(gray)) / gray.total();

        // Save image
        cv::imwrite("images/pic_sat_" + int2str0(sat_id, 3) + "_" +
            int2str0(step_no, 6) + ".png", image);
        
        // Process image
        if (blp <= max_blp) {

            // Populate transmission tr_last
            tr_last.sat_id = sat_id;
            tr_last.step = step_no;
            tr_last.t = t;
            tr_last.sift = cc.sift(image);
            tr_last.dist_x = dist_x;

            // Run image-based cross-calibration
            dist_x = cc.run(tr_last, *filt);
    
        }

    }

    // Store updated state estimate
    states_est.back() = dist_x;
    
    // Set attitude control torque
    sat_state x_est, x_nadir;
    x_est.X = dist_x.mean;
    x_nadir = x_est;
    x_nadir.set_nadir();
    vec<3> tc = att_ctrl.tau(x_est.qb(), x_est.w(), x_nadir.qb(), x_nadir.w());
    roamps_("TC", tc.data());    
    roamps_("JP", J.data()); 

    sat_state x_tru_nadir = x_tru;
    x_tru_nadir.set_nadir();

    // Get pointing error
    vec<2> att_err_k;
    att_err_k.x() = rad2deg(x_tru.qb().angularDistance(x_tru_nadir.qb()));
    att_err_k.y() = rad2deg((x_tru.w() - x_tru_nadir.w()).norm()); 
    att_err.push_back(att_err_k);

    // Update step
    step_no++;

    // Get next time
    double t_next = step_no * dt;
    times.push_back(t_next);
        
    // Propagate true state
    sat_state x_tru_next;
    x_tru_next.X = dyn_tru.propagate_random(t, t_next, x_tru.X, rnd);
    states_tru.push_back(x_tru_next);

    // Predict state distribution
    filter::dist dist_x_next = filt->predict(t, t_next,
            dist_x, dist_w, dyn_tru); 
    states_est.push_back(dist_x_next);

}
