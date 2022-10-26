#include <sat_state.hpp>

#include <angles.hpp>
#include <roam.h>

#include <cmath>

const int sat_state::ND;

const int sat_state::N;

const int sat_state_err::N;

const int sat_state_err::ND;

const int img_state_diff::N;

vec<> sat_dyn::propagate(double ti, double tf, cvec<> xi, cvec<> w) {

    static const int nt = 1;

    int dr = include_drag;

    vec<3> Fd, Td;
    Fd = w.head<3>();
    Td = w.tail<3>();

    roamos_("DR", &dr);
    roamps_("FD", Fd.data());
    roamps_("TD", Td.data());

    sat_state s1, s2;
    s1.X = xi;
    s2.X = xi;

    vec<13> xdi, xdf;
    xdi.head<9>() = s1.X.head<9>(); 
    xdi.tail<4>() = s1.qvb();

    roam_(&ti, xdi.data(), &nt, &tf, xdf.data());

    s2.X.head<9>() = xdf.head<9>();
    s2.qvb(xdf.tail<4>());

    return s2.X;

}

vec<> sat_dyn::propagate_random(double ti, double tf, cvec<> xi, rando& rnd) {
    vec<6> w = rnd.mvn<6>();
    w.head<3>() *= stdf;
    w.tail<3>() *= stdt;
    return propagate(ti, tf, xi, w);
}

vec<> sat_dyn::f(double ti, double tf, cvec<> xi, cvec<> w) {
    return propagate(ti, tf, xi, w);
}

mat<> sat_dyn::cov() {
    mat<> P(6, 6);
    P.setIdentity();
    P.topLeftCorner    <3,3>() *= (stdf * stdf);
    P.bottomRightCorner<3,3>() *= (stdt * stdt);
    return P;
}

void sat_state::set_coe(const coe& coe_in) {
    coe2rv_(coe_in.y(), x());
}

coe sat_state::get_coe() {
    coe coe_out;
    rv2coe_(x(), coe_out.y());
    return coe_out;
}

quat sat_state::qa() const {

    vec<3> n1, n2, n3, h;

    h = r().cross(v());

    n3 = -r().normalized();
    n2 = -h.normalized();
    n1 = n2.cross(n3);

    mat<3,3> R;

    R << n1, n2, n3;

    return quat(R);

}

void sat_state::set_nadir() {

    qb(qc().conjugate() * qa());
    
    vec<3> h = r().cross(v());

    w() = qb().conjugate()._transformVector(h) / r().squaredNorm();

}

void sat_state::set_ideal_cam(double f_ideal, const quat& qc_ideal) {
    c().setZero();
    f() = f_ideal;
    qc(qc_ideal);
}

sat_state_err sat_state_err::diff(const sat_state& x1, const sat_state& x2) {

    sat_state_err err;

    err.dr() = (x1.r() - x2.r()).norm() * 1000;
    err.dv() = (x1.v() - x2.v()).norm() * 1000;
    err.dw() = rad2deg((x1.w() - x2.w()).norm());
    err.dba() = rad2deg(x1.qb().angularDistance(x2.qb()));
    err.dca() = rad2deg(x1.qc().angularDistance(x2.qc()));
    err.df() = fabs(x1.f() - x2.f());

    err.dc() = (x1.c() - x2.c()).cwiseAbs();

    return err;
}

sat_state_err sat_state_err::diff(cvec<> x1, cvec<> x2) {
    sat_state state1, state2;
    state1.X = x1;
    state2.X = x2;
    return diff(state1, state2);
}

sat_state sat_state_randomizer::randomize(const sat_state& state, rando& rand) {

    sat_state randst;

    randst.r() = state.r() + stdr * rand.mvn<3>();
    randst.v() = state.v() + stdv * rand.mvn<3>();
    randst.w() = state.w() + stdw * rand.mvn<3>();

    randst.qb(state.qb() * rand.rq(stdba));
    randst.qc(state.qc() * rand.rq(stdca));

    randst.f() = state.f() + stdf * rand.norm();

    randst.c() = state.c() + stdc * rand.mvn<sat_state::ND>();

    return randst;

}

mat<> sat_state_randomizer::cov() {
    vec<> p(sat_state::N);
    p.segment<3>(0).setConstant(stdr); 
    p.segment<3>(3).setConstant(stdv);
    p.segment<3>(6).setConstant(stdw);
    p.segment<3>(9).setConstant(stdba);
    p.segment<3>(12).setConstant(stdca);
    p(15) = stdf;
    p.segment<sat_state::ND>(16).setConstant(stdc);
    return p.cwiseAbs2().asDiagonal();
}


img_state_diff::img_state_diff(double t1, double t2, const sat_state& s1,
        const sat_state& s2) {

    vec<3> reci1, reci2, recef1, recef2, dr_ecef, dr_eci;

    reci1 = s1.r();
    reci2 = s2.r();

    eci2ef_(&t1, reci1.data(), recef1.data());
    eci2ef_(&t2, reci2.data(), recef2.data());

    dr_ecef = recef2 - recef1;

    ef2eci_(&t1, dr_ecef.data(), dr_eci.data());

    quat q1 = s1.qb() * s1.qc();
    quat q2 = s2.qb() * s2.qc();

    dx.head<3>() = q1.inverse()._transformVector(dr_eci);

    dx.segment<3>(3) = quat2rod(q2 * q1.inverse()); 

    dx(6) = s2.f() - s1.f();

    dx(7) = 1;

    dx.tail<sat_state::ND>() = s2.c() - s1.c();

}
