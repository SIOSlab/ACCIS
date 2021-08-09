#include <angles.hpp>
#include <cross_cal.hpp>
#include <filter.hpp>
#include <house.hpp>
#include <Landsat.hpp>
#include <sat_meas.hpp>
#include <sat_state.hpp>
#include <ukf.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

double cond(cmat<> A) {
    Eigen::JacobiSVD<mat<>> svd(A);
    return svd.singularValues()(0)
        / svd.singularValues()(svd.singularValues().size()-1);
}

void test_accis(filter::base& filt, const std::string& dir) {

    coe coe1, coe2;

    constexpr double RE = 6378;

    constexpr double h = 400;

    coe1.a() = RE + h;
    coe1.e() = 0;
    coe1.i() = 0;
    coe1.w() = -44; //-46;//-39;//-38;
    coe1.u() = 0;
    coe1.f() = 0;

    coe2 = coe1;
   
    constexpr double df = 0.5;

    coe2.f() -= df;

    // double epsilon = atan(sin(deg2rad(df)) / (h/RE + 1 - cos(deg2rad(df))));

    sat_state state1_ideal, state2_ideal, state1_tru, state2_tru;

    state1_ideal.set_coe(coe1);
    state2_ideal.set_coe(coe2);

    sat_cam cam;

    cam.widp = 1000;
    cam.lenp = 1000; // 1000

    cam.u = 2000;    
    cam.rho = 1;
    cam.A = 500;

    state1_ideal.set_ideal_cam(cam.u);
    state2_ideal.set_ideal_cam(cam.u);
    
    state1_ideal.set_nadir();
    state2_ideal.set_nadir();

    /*
    Eigen::AngleAxisd aa1(-epsilon, vec<3>::UnitY()),
                      aa2( epsilon, vec<3>::UnitY());

    quat q1(aa1), q2(aa2);

    state1_ideal.qc(q1 * state1_ideal.qc());
    state2_ideal.qc(q2 * state2_ideal.qc());
    */

    rando rdo;

    sat_state_randomizer rzer;    

    rzer.stdr = 0.1;
    rzer.stdv = 0.1;
    rzer.stdw = deg2rad(0.1);
    rzer.stdba = deg2rad(0.1);
    rzer.stdca = deg2rad(0.1);
    rzer.stdf = 1;
    rzer.stdc = 0.01;

    state1_tru = rzer.randomize(state1_ideal, rdo);
    state2_tru = rzer.randomize(state2_ideal, rdo);

    /*
    std::cout << state1_ideal.X << std::endl;
    std::cout << "------------" << std::endl;
    std::cout << state1_tru.X << std::endl;
    */

    const int N = sat_state::N;

    filter::dist dist1(N), dist2(N);

    dist1.mean = state1_ideal.X;
    dist2.mean = state2_ideal.X;

    dist1.cov = rzer.cov();
    dist2.cov = rzer.cov();    

    vec<> state_skew(sat_state::N), state_kurt(sat_state::N);
    state_skew.setZero();
    state_kurt.setConstant(3);
    dist1.par.push_back(state_skew);
    dist2.par.push_back(state_skew);
    dist1.par.push_back(state_kurt);
    dist2.par.push_back(state_kurt);

    std::vector<sat_state> states1_tru, states2_tru;
    states1_tru.push_back(state1_tru);
    states2_tru.push_back(state2_tru);

    std::vector<filter::dist> states1_est, states2_est;
    states1_est.push_back(dist1);
    states2_est.push_back(dist2);

    std::vector<double> times;
    times.push_back(0);

    sat_dyn dyn;
    dyn.stdf = 10;
    dyn.stdt = 1;
    filter::dist dist_w(6);
    dist_w.mean.setZero();
    dist_w.cov = dyn.cov();
    vec<> skew_w(6), kurt_w(6);
    skew_w.setZero();
    kurt_w.setConstant(3);
    dist_w.par.push_back(skew_w);
    dist_w.par.push_back(kurt_w);

    sat_meas::gps h_gps;
    sat_meas::star_tracker h_str;

    filter::dist gps_err(6), str_err(3);
    gps_err.mean.setZero();
    str_err.mean.setZero();
    gps_err.cov = h_gps.cov();
    str_err.cov = h_str.cov();
    
    vec<> skew_gps(6), kurt_gps(6);
    skew_gps.setZero();
    kurt_gps.setConstant(3);
    gps_err.par.push_back(skew_gps);
    gps_err.par.push_back(kurt_gps); 

    vec<> skew_str(3), kurt_str(3);
    skew_str.setZero();
    kurt_str = sat_meas::star_tracker::kurt();
    str_err.par.push_back(skew_str);
    str_err.par.push_back(kurt_str);

    filter::dist dist_w_kp(2);
    dist_w_kp.mean.setZero();
    dist_w_kp.cov.setIdentity();
    dist_w_kp.cov *= 100;
    vec<2> skew_w_kp, kurt_w_kp;
    skew_w_kp.setZero();
    kurt_w_kp.setConstant(3);
    dist_w_kp.par.push_back(skew_w_kp);
    dist_w_kp.par.push_back(kurt_w_kp);
        
    cross_cal cc;
    cc.dt_max = 20;
    cc.kp_r_max = 100;
    cc.kp_d_max = 0.1; // 0.05;
    cc.cam = cam;
    cc.dist_w_kp = dist_w_kp;
    cc.num_sift_pts = 1000;

    for (int k = 0; k < 100; k++) {

        double t = k * 0.2;

        if (k > 0) {

            filter::dist dist_xp1 = filt.predict(times.back(), t,
                   states1_est.back(), dist_w, dyn); 
            filter::dist dist_xp2 = filt.predict(times.back(), t,
                   states2_est.back(), dist_w, dyn); 
            
            states1_est.push_back(dist_xp1);
            states2_est.push_back(dist_xp2);
            
            sat_state s1, s2;
            s1 = states1_tru.back().propagate(times.back(), t);
            s2 = states2_tru.back().propagate(times.back(), t);
            states1_tru.push_back(s1);
            states2_tru.push_back(s2);           

            times.push_back(t);
        }

        filter::dist dist_x1 = states1_est.back(); 
        filter::dist dist_x2 = states2_est.back(); 

        vec<> z1, z2;
        
        z1 = h_gps.H(t, states1_tru.back().X, rdo); 
        z2 = h_gps.H(t, states2_tru.back().X, rdo); 
        
        dist_x1 = filt.update(t, z1, dist_x1, gps_err, h_gps);
        dist_x2 = filt.update(t, z2, dist_x2, gps_err, h_gps);

        z1 = h_str.H(t, states1_tru.back().X, rdo); 
        z2 = h_str.H(t, states2_tru.back().X, rdo); 
        
        dist_x1 = filt.update(t, z1, dist_x1, str_err, h_str);
        dist_x2 = filt.update(t, z2, dist_x2, str_err, h_str);

        if ((k % 5) == 0)
            std::cout << "T = " << t << " s" << std::endl;
        
        if (k == 25 || k == 75) {
            
            cv::Mat img1 = cam.real_image(t, states1_tru.back());
            cv::Mat img2 = cam.real_image(t, states2_tru.back());

            cv::imwrite("images/pic" + std::to_string(k) + "_sat1.png", img1);
            cv::imwrite("images/pic" + std::to_string(k) + "_sat2.png", img2);

            cross_cal::transmission tr1, tr2;

            tr1.sat_id = 1;
            tr2.sat_id = 2;

            tr1.t = t;
            tr2.t = t;

            tr1.sift = cc.sift(img1);
            tr2.sift = cc.sift(img2);

            std::cout << "Number of SIFT points: " << tr1.sift.num_pts
                << ", " << tr2.sift.num_pts << std::endl;

            tr1.dist_x = dist_x1;
            tr2.dist_x = dist_x2;

            dist_x1 = cc.run(tr1, filt);
            dist_x2 = cc.run(tr2, filt);

            std::cout << "hello" << std::endl;
            
        }

//        std::cout << "Cond: " << cond(dist_x1.cov) << ", " << cond(dist_x2.cov) << std::endl;

//        std::cout << "Diag: " << dist_x1.cov.diagonal() << std::endl;

        states1_est.back() = dist_x1;
        states2_est.back() = dist_x2;

    }

    sat_state_save("results/" + dir + "/sat1", times, states1_tru, states1_est);
    sat_state_save("results/" + dir + "/sat2", times, states2_tru, states2_est);

}

int main(int argc, char** argv) {

    try {

        char f = argv[1][0];
  
        if (f == 'h') {
            house the_house;
            test_accis(the_house, "HOUSE");
        } else if (f == 'u') {
            ukf the_ukf;
            test_accis(the_ukf, "UKF");
        }

     } catch (const std::exception& e) {

         std::cout << e.what() << std::endl;

    }

    return 0;

}
