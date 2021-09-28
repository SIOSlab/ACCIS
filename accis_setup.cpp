#include "accis.hpp"

#include "angles.hpp"
#include "mat.hpp"
#include "ukf.hpp"

void accis_sat::setup() {

    using namespace pydict;

    step_no = 0;

    sat_id = getset<std::string>(par, "Satellite ID", "XXX");

    int seed = getset<int>(par, "Random Number Generator Seed", 0);
    rnd = rando(seed);    

    std::string filter_type = getset<std::string>(par, "Filter Type", "UKF");
    if (filter_type == "UKF")
        filt.reset(new ukf());
    else
        show_error("Fiter Type " + filter_type + " not recognized");

    dt = getset<double>(par, "Time Step (s)", 0.1);

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
            "Disturbance Torque StD (N*m) - Ground Truth", 1);
    dyn_tru.stdt = getset<double>(par,
            "Disturbance Force StD (N) - Ground Truth", 0.1);
    dyn_filt.stdf = getset<double>(par,
            "Disturbance Torque StD (N*m) - Filter Model", 10);
    dyn_filt.stdt = getset<double>(par,
            "Disturbance Force StD (N) - Filter Model", 1);

    dist_w = filter::dist(6);
    dist_w.mean.setZero();
    dist_w.cov = dyn_filt.cov();

    cadence_gps = getset<int>(par, "GPS Measurement Cadence", 10);
    cadence_str = getset<int>(par, "Star Tracker Measurement Cadence", 10);
    cadence_img = getset<int>(par, "Imaging Cadence", 10);

}
