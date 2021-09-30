#include <sat_state.hpp>

#include <angles.hpp>
#include <roam.h>

#include <cmath>

const int sat_state::ND;

const int sat_state::N;

const int sat_state_err::N;

const int sat_state_err::ND;

vec<> sat_dyn::propagate(double ti, double tf, cvec<> xi, cvec<> w) {

    static const int nt = 1;

    int dr = include_drag;

    vec<3> Fd, Td;
    Fd = w.head<3>();
    Td = w.tail<3>();

    roamos_("DR", &dr);
    roamps_("FD", Fd.data());
    roamps_("TD", Td.data());

    vec<> Xi = xi;
    vec<> Xf = Xi;

    roam_(&ti, Xi.data(), &nt, &tf, Xf.data());

    return Xf;

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

void sat_state::set_nadir() {

    vec<3> n1, n2, n3, h;

    h = r().cross(v());

    n3 = -r().normalized();
    n2 = -h.normalized();
    n1 = n2.cross(n3);

    mat<3,3> R;

    R << n1, n2, n3;

    quat qR(R);

    qb(qc().inverse() * qR);

    w() = -(h.norm() / r().squaredNorm()) * n2;

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
    p.segment<4>(9).setConstant(stdba);
    p.segment<4>(13).setConstant(stdca);
    p(17) = stdf;
    p.segment<sat_state::ND>(18).setConstant(stdc);
    return p.cwiseAbs2().asDiagonal();
} 
