#include "accis.hpp"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp> 

#include <string>

void accis_sat::step() {

    // Get current time
    double t = times.back();

    // Get true state
    sat_state x_tru = states_tru.back();

    // Get current state estimate distribution
    filter::dist dist_x = states_est.back();

    // Update state estimate -- GPS
    
    // Update state estimate -- Star tracker
    
    // Update state estimate -- Image-based
    if (step_no % cadence_img == 0) {

        cv::Mat image = cam.real_image(t, x_tru);

        cv::imwrite("images/pic_" + sat_id + "_" +
                std::to_string(step_no) + ".png", image);

    }

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
