#include "accis.hpp"

void accis_sat::setup() {

    using namespace pydict;

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

    sat_state x0;
    x0.set_coe(orb);

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

    int seed = getset<int>(par, "Random Number Generator Seed", 0);
    rnd = rando(seed);    

}
