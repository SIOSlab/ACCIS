#include "generator.hpp"

#include "angles.hpp"
#include "eigen_csv.hpp"
#include "format.hpp"
#include "rando.hpp"
#include "sifter.hpp"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp> 
#include <opencv2/imgproc.hpp>

#include <cmath>
#include <iostream>

generator::generator() {
    setup();
}

void generator::set_param(const pydict::dict& d) {
    par = d;
    setup();
}

pydict::dict generator::get_param() {
    return par;
}

void generator::gen_imgs() {

    // Equatorial radius of Earth
    const double RE = 6378;

    // Time
    const double t = 0;

    // Random number generator
    rando rnd(seed);

    // Generate & process states & images
    for (int i = 1; i <= max_imgs; i++) {

        std::cout << "Image " << i << " of " << max_imgs << std::endl;

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
        s.set_ideal_cam(cam.u);

        // Set orbital elements
        s.set_coe(orbit);

        // Set to nadir-pointing
        s.set_nadir();

        // Randomize state
        s = rzer.randomize(s, rnd);
        
        // Get image
        cv::Mat img = cam.real_image(t, s); 

        // Save image
        cv::imwrite("gen/img_" + int2str0(seed, 6) + "_"
                + int2str0(i, 6) + ".png", img);

        // Save state
        eigen_csv::write(s.X, "gen/state_" + int2str0(seed, 6) + "_"
                + int2str0(i, 6) + ".csv");

    }

}

mat<> generator::proc_imgs(const std::vector<std::string>& img_files,
        const std::vector<std::string>& state_files) {

    // Time
    const double t = 0;
    
    // SIFT points
    std::vector<sifter::points> key_pts;

    for (int i = 0; i < img_files.size(); i++) {

        // Read image
        cv::Mat img = cv::imread(img_files[i]);

        // Read state
        sat_state s;
        eigen_csv::read(state_files[i], false, false, s.X);

        // Percentage of black pixels
        cv::Mat gray;
        cv::cvtColor(img, gray, cv::COLOR_RGB2GRAY);
        double blp = 100.0 - (100.0 * cv::countNonZero(gray)) / gray.total();

        // Apply SIFT 
        if (blp < max_blp) {

            // SIFT key points
            sifter::points pts = sifter::sift(t, s, img, num_pts);

            // Save key points
            key_pts.push_back(pts);

        }

    }

    // Matched key points & state differences
    std::vector<vec<4>> points_query, points_train;
    std::vector<vec<img_state_diff::N>> dxs;

    // Match SIFT key points
    for (int i = 0; i < key_pts.size(); i++) {

        for (int j = 0; j < key_pts.size(); j++) {

            if (i != j) {

                sifter::matches sm = sifter::match(key_pts[i], key_pts[j], cam,
                        max_dist, max_kp_dist);

                for (int k = 0; k < sm.num_pts; k++) {

                    points_query.push_back(sm.query[k]);
                    points_train.push_back(sm.train[k]);

                    dxs.push_back(sm.dx);

                }

            }

        }

    }

    // Make table of states & key points
    int num_pts = dxs.size(); 
    mat<> table(num_pts, img_state_diff::N + 8);
    vec<img_state_diff::N + 8> r;
    for (int k = 0; k < num_pts; k++) {
        r << points_query[k], points_train[k], dxs[k];
        table.row(k) = r;
    }

    return table;

}

void generator::setup() {

    using namespace pydict;

    seed = getset<int>(par, "Seed", 0);

    max_imgs = getset<int>(par, "Max. Number of Images", 100);

    num_pts = getset<int>(par, "Number of Key Points", 100);

    max_blp = getset<double>(par, "Max. Percentage of Black Pixels", 5);

    avg_alt = getset<double>(par, "Average Altitude (km)", 500);
    var_alt = getset<double>(par, "Altitude Variation (km)", 100);

    cam.widp = getset<double>(par, "Camera Image Width (pixels)", 1000);
    cam.lenp = getset<double>(par, "Camera Image Length (pixels)", 1000);
    cam.u    = getset<double>(par, "Camera Focal Length (mm)", 2000);
    cam.A    = getset<double>(par, "Camera Aperture (mm)", 500);
    cam.rho  = getset<double>(par, "Camera Pixel Density (pixels/mm)", 1);

    rzer.stdr = getset<double>(par, "Position StD (km)", 10);
    rzer.stdv = getset<double>(par, "Velocity StD (km/s)", 1);
    rzer.stdw = getset<double>(par, "Angular Velocity StD (rad/s)",
        deg2rad(0.1));
    rzer.stdba = getset<double>(par, "Body Attitude StD (rad)",
        deg2rad(1));
    rzer.stdca = getset<double>(par, "Camera Attitude StD (rad)",
        deg2rad(1));
    rzer.stdf = getset<double>(par, "Camera Focal Length StD (mm)", 10);
    rzer.stdc = getset<double>(par, "Camera Distortion Parameter StD", 0.1);

    max_dist = getset<double>(par, "Max. Key Point Angular Distance (deg)", 1);
    
    max_kp_dist = getset<double>(par, "Max. Key Point Pixel Distance", 100);

}
