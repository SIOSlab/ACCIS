#include "accis.hpp"

#include "angles.hpp"
#include "eigen_csv.hpp"
#include "house.hpp"
#include "mat.hpp"
#include "sqrtukf.hpp"
#include "ukf.hpp"

void accis_sat::setup() {

    using namespace pydict;

    step_no = 0;

    sat_id = getset<int>(par, "Satellite ID", 0);

    int seed = getset<int>(par, "Random Number Generator Seed", sat_id);
    rnd = rando(seed);    

    std::string filter_type = getset<std::string>(par, "Filter Type", "UKF");
    if (filter_type == "UKF")
        filt.reset(new ukf());
    else if (filter_type == "HOUSE")
        filt.reset(new house());
    else if (filter_type == "SQRTUKF")
        filt.reset(new sqrtukf());
    else
        show_error("Fiter Type " + filter_type + " not recognized");

    dt = getset<double>(par, "Time Step (s)", 1);

    cam.widp = getset<double>(par, "Camera Image Width (pixels)", 1000);
    cam.lenp = getset<double>(par, "Camera Image Length (pixels)", 1000);
    cam.u    = getset<double>(par, "Camera Focal Length (mm)", 2000);
    cam.A    = getset<double>(par, "Camera Aperture (mm)", 500);
    cam.rho  = getset<double>(par, "Camera Pixel Density (pixels/mm)", 1);

    coe orb;
    orb.a() = getset<double>(par, "Orbit Semi-Major Axis (km)", 6750);
    orb.e() = getset<double>(par, "Orbit Eccentricity", 0);
    orb.i() = getset<double>(par, "Orbit Inclination (deg)", 0);
    orb.w() = getset<double>(par, "Orbit RAAN (deg)", 0);
    orb.u() = getset<double>(par, "Orbit Argument of Perigee (deg)", 0);
    orb.f() = getset<double>(par, "Orbit True Anomaly at Epoch (deg)", 0);

    quat q_id = quat::Identity();
    quat qc0;
    qc0.coeffs() = getset<vec<4>>(par, "Camera attitude w.r.t. body", q_id.coeffs());

    sat_state x0_ideal;
    x0_ideal.set_coe(orb);
    x0_ideal.set_ideal_cam(cam.u, qc0);
    x0_ideal.set_nadir();    

    sat_state_randomizer rzer;

    rzer.stdr = getset<double>(par, "Initial Position StD (km)", 0.1);
    rzer.stdv = getset<double>(par, "Initial Velocity StD (km/s)", 0.1);
    rzer.stdw = getset<double>(par, "Initial Angular Velocity StD (rad/s)",
        deg2rad(0.1));
    rzer.stdba = getset<double>(par, "Initial Body Attitude StD (rad)",
        deg2rad(0.1));
    rzer.stdca = getset<double>(par, "Camera Attitude StD (rad)",
        deg2rad(0.1));
    rzer.stdf = getset<double>(par, "Camera Focal Length StD (mm)", 1);
    rzer.stdc = getset<double>(par, "Camera Distortion Parameter StD", 0.01);

    sat_state x0 = rzer.randomize(x0_ideal, rnd); 

    states_tru.clear();
    states_tru.push_back(x0);

    filter::dist dist_x0(sat_state::N);
    dist_x0.mean = x0_ideal.X;
    dist_x0.cov = rzer.cov();

    states_est.clear();
    states_est.push_back(dist_x0);

    times.clear();
    times.push_back(0);

    dyn_tru.stdf = getset<double>(par,
            "Disturbance Force StD (N) - Ground Truth", 1E-3);
    dyn_tru.stdt = getset<double>(par,
            "Disturbance Torque StD (N*m) - Ground Truth", 0.1);
    dyn_tru.include_drag = true;
    dyn_tru.log_rtol = -10;
    dyn_tru.log_atol = -10;

    dyn_filt.stdf = getset<double>(par,
            "Disturbance Force StD (N) - Filter Model", 10);
    dyn_filt.stdt = getset<double>(par,
            "Disturbance Torque StD (N*m) - Filter Model", 1);
    dyn_filt.include_drag = false;
    dyn_filt.log_rtol = -6;
    dyn_filt.log_atol = -6;

    dist_w = filter::dist(6);
    dist_w.mean.setZero();
    dist_w.cov = dyn_filt.cov();
    vec<> skew_w(6), kurt_w(6);
    skew_w.setZero();
    kurt_w.setConstant(3);
    dist_w.par.push_back(skew_w);
    dist_w.par.push_back(kurt_w);

    cadence_gps = getset<int>(par, "GPS Measurement Cadence", 1);
    cadence_str = getset<int>(par, "Star Tracker Measurement Cadence", 1);
    cadence_gyr = getset<int>(par, "Gyroscope Measurement Cadence", 1);
    cadence_img = getset<int>(par, "Imaging Cadence", 60);

    gps_err = filter::dist(6);
    str_err = filter::dist(3);
    gyr_err = filter::dist(3);

    gps_err.mean.setZero();
    str_err.mean.setZero();
    gyr_err.mean.setZero();
    gps_err.cov = h_gps.cov();
    str_err.cov = h_str.cov();
    gyr_err.cov = h_gyr.cov();
    
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

    vec<3> J_default;
    J_default << 100, 100, 100;
    J = getset<vec<3>>(par, "Principal Moments of Inertia (kg*m^2)", J_default);
    att_ctrl.q0 = x0_ideal.qb();
    att_ctrl.wc = x0_ideal.w();
    att_ctrl.kp = getset<double>(par, "PD Constant kp", 10);
    att_ctrl.kd = getset<double>(par, "PD Constant kd", 100);
    att_ctrl.J = J.asDiagonal();

    mat<4,4> sift_cov;
    mat<4,img_state_diff::N> sift_h_mat;

    eigen_csv::read("sift_coefs/cov.csv", false, false, sift_cov);    

    filter::dist dist_w_kp(4);
    dist_w_kp.mean.setZero();
    dist_w_kp.cov = sift_cov;

    cc.train.clear();
    cc.dist_w_kp = dist_w_kp;
    cc.num_sift_pts = getset<int>(par, "Number of SIFT Points", 1000);
    cc.dt_max = getset<double>(par, "Cross Calibration Time Memory (s)", 3600); 
    cc.max_dist = getset<double>(par, "Keypoint Max. Distance", 100);
    cc.cam = cam;

    max_blp = getset<double>(par, "Max. Percentage of Black Pixels", 5);

    tr_last.step = -10;

}
