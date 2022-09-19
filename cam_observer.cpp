#include "cam_observer.hpp"

#include "cross_cal.hpp"
#include "sat_state.hpp"

void cam_observer::set_param(const pydict::dict& d1, const pydict::dict& d2) {
    sat1.set_param(d1);
    sat2.set_param(d2);
}

void cam_observer::set_pts(const mat<>& pts1_in, const mat<>& pts2_in) {
    pts1 = pts1_in;
    pts2 = pts2_in;
}

vec<> cam_observer::h(cvec<> dx_cam) {

    sat_state s1 = sat1.states_tru.back();
    sat_state s2 = sat2.states_tru.back();

    s1.pc() += dx_cam.head<3>();
    s2.pc() += dx_cam.segment<3>(3);
    
    s1.c() += dx_cam.segment<sat_state::ND>(6);
    s2.c() += dx_cam.tail<sat_state::ND>();

    vec<> x1(2 * sat_state::N), x2(2 * sat_state::N);
    x1 << s1.X, s2.X;
    x2 << s2.X, s1.X;
    
    double t1 = sat1.times.back();
    double t2 = sat2.times.back();
    
    int n1 = pts1.cols();
    int n2 = pts2.cols();

    mat<4> z1(4, n1); 
    mat<4> z2(4, n2); 

    for (int i = 0; i < n1; i++)
        z1.col(i) = cross_cal_meas(t1, t2, s1, s2, sat1.cam, pts1.col(i));

    for (int i = 0; i < n2; i++)
        z2.col(i) = cross_cal_meas(t2, t1, s2, s1, sat2.cam, pts2.col(i));

    vec<> z(4 * (n1 + n2));
    z << z1.reshaped(), z2.reshaped();

    return z;

}
