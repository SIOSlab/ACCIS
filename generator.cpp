#include "generator.hpp"

#include "angles.hpp"
#include "format.hpp"
#include "rando.hpp"
#include "sifter.hpp"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp> 
#include <opencv2/imgproc.hpp>

#include <cmath>

generator::generator() {
    setup();
}

generator::generator(const pydict::dict& d) {
    par = d;
    setup();
}

void generator::set_param(const pydict::dict& d) {
    par = d;
    setup();
}

pydict::dict generator::get_param() {
    return par;
}

mat<> generator::run() {

    // Equatorial radius of Earth
    const double RE = 6378;

    // Random number generator
    rando rnd(seed);

    // Satellite states & SIFT points
    std::vector<sat_state> states;
    std::vector<sifter::points> key_pts;

    // Generate & process states & images
    for (int i = 0; i < max_imgs; i++) {

        // Generate orbital elements
        coe orbit;
        orbit.a() = RE + avg_alt + var_alt * rnd.unif();
        orbit.e() = 0;
        orbit.i() = rad2deg(acos(rnd.unif()));
        orbit.w() = 180 * rnd.unif();
        orbit.u() = 180 * rnd.unif();
        orbit.f() = 180 * rnd.unif();

        // Satellite state
        sat_state s;

        // Set ideal camera parameters
        s.set_ideal_cam(f_ideal);

        // Set orbital elements
        s.set_coe(orbit);

        // Set to nadir-pointing
        s.set_nadir();

        // Randomize state
        s = rzer.randomize(s, rnd);
        
        // Get images
        cv::Mat img = cam.real_image(0, s); 

        // Save image
        cv::imwrite("images/gen_pic_" + int2str0(i, 6) + ".png", img);

        // Percentages of black pixels
        cv::Mat gray;
        cv::cvtColor(img, gray, cv::COLOR_RGB2GRAY);
        double blp = 100.0 - (100.0 * cv::countNonZero(gray)) / gray.total();

        // Apply SIFT 
        if (blp < max_blp) {

            // SIFT key points
            sifter::points pts = sifter::sift(img, num_pts);

            // Save state & key points
            states.push_back(s);
            key_pts.push_back(pts);

        }

    } 

    // Matched states & key points
    std::vector<sat_state> states_query, states_train;
    std::vector<vec<4>>    points_query, points_train;

    // Match SIFT key points
    for (int i = 0; i < states.size(); i++) {

        for (int j = 0; j < i; j++) {

            sifter::matches sm = sifter::match(key_pts[i], key_pts[j], dmax);

            for (int k = 0; k < sm.num_pts; k++) {

                states_query.push_back(states[i]);
                states_train.push_back(states[j]);
    
                points_query.push_back(sm.query[k]);
                points_train.push_back(sm.train[k]);

            }

        }

    }

    // Make table of states & key points
    int num_pts = states_query.size(); 
    mat<> table(num_pts, 2*sat_state::N + 8);
    for (int i = 0; i < num_pts; i++)
        table.row(i) << states_query[i].X, states_train[i].X,
                        points_query[i],   points_train[i];

    // Return table
    return table;

}

void generator::setup() {


}
