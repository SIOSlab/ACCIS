#include "generator.hpp"

#include "angles.hpp"
#include "cross_cal.hpp"
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

mat<> generator::run() {

    // Time
    const double t = 0;

    // Random number generator
    rando rnd(seed);

    // Camera center
    vec<2> c = cam.center();

    // Key point errors
    std::vector<vec<4>> kp_err;
    
    // Generate & process states & images
    for (int i = 1; i <= max_imgs; i++) {

        std::cout << "Image Pair " << i << " of " << max_imgs << std::endl;

        double blp;
        double dist;

        sat_state s1;
        sat_state s2;

        cv::Mat img1;
        cv::Mat img2;

        // Generate first satellite state & image
        std::cout << " -- Generating first image" << std::endl;
        do {
            
            s1 = gen_state(rnd); 

            img1 = cam.real_image(t, s1);

            blp = sat_cam::blp(img1);

        } while (blp > max_blp);

        // Generate second satellite state & image
        std::cout << " -- Generating second image" << std::endl;
        do {

            do {
                    
                s2 = gen_state(rnd); 
                
                dist = cam.pt_dist(t, t, s1, s2, c, c);

            } while (dist > max_dist);

            img2 = cam.real_image(t, s2);

            blp = sat_cam::blp(img2);

        } while (blp > max_blp);

        // Save image
        cv::imwrite("images/gen_pic_" + int2str0(i, 6) + "_1.png", img1);
        cv::imwrite("images/gen_pic_" + int2str0(i, 6) + "_2.png", img2);

        // SIFT key points
        sifter::points pts1 = sifter::sift(t, s1, img1, num_pts);
        sifter::points pts2 = sifter::sift(t, s2, img2, num_pts);

        // Match SIFT key points
        sifter::matches sm = sifter::match(pts1, pts2, cam, max_dist,
                max_kp_dist);

        for (int k = 0; k < sm.num_pts; k++) {

            vec<4> z  = sm.query[k];
            vec<4> zr = sm.train[k];        
            
            vec<4> zc = cross_cal_meas(t, t, s1, s2, cam, zr);

            kp_err.push_back(z - zc);
        
        } 

    }

    // Number of key point pairs
    int num_pts = kp_err.size(); 
    std::cout << num_pts << " key point pairs generated" << std::endl;  

    // Make table of key point errors
    mat<> w(4, num_pts);
    for (int k = 0; k < num_pts; k++) 
        w.col(k) = kp_err[k];

    // Error covariance
    mat<4,4> Pww = w * w.transpose() / num_pts; 

    // Return covariance
    return Pww;

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

sat_state generator::gen_state(rando& rnd) {

    // Equatorial radius of Earth
    const double RE = 6378;

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

    return s;

}
