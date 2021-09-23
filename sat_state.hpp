#ifndef SAT_STATE_HPP
#define SAT_STATE_HPP

#include <filter.hpp>
#include <mat.hpp>
#include <rando.hpp>

#include <string>
#include <vector>

class coe {

    public:

    vec<6> Y;

    double* y() { return Y.data(); }

    const double* y() const { return Y.data(); }

    double& a() { return Y(0); }
    double& e() { return Y(1); }
    double& i() { return Y(2); }
    double& w() { return Y(3); }
    double& u() { return Y(4); }
    double& f() { return Y(5); }

    const double& a() const { return Y(0); }
    const double& e() const { return Y(1); }
    const double& i() const { return Y(2); }
    const double& w() const { return Y(3); }
    const double& u() const { return Y(4); }
    const double& f() const { return Y(5); }

};

class sat_state {

    public:

    static const int ND = 3;

    static const int N = 18 + ND; 

    vec<N> X;

    double* x() { return X.data(); }

    const double* x() const { return X.data(); }

    rvec<3> r  () { return X.head<3>();      }
    rvec<3> v  () { return X.segment<3>(3);  }
    rvec<3> w  () { return X.segment<3>(6);  }
    rvec<4> qbv() { return X.segment<4>(9);  }
    rvec<4> qcv() { return X.segment<4>(13); }
    double& f  () { return X(17);            }
    rvec<ND> c () { return X.tail<ND>();     }

    cvec<3> r      () const { return X.head<3>();      }
    cvec<3> v      () const { return X.segment<3>(3);  }
    cvec<3> w      () const { return X.segment<3>(6);  }
    cvec<4> qbv    () const { return X.segment<4>(9);  }
    cvec<4> qcv    () const { return X.segment<4>(13); }
    const double& f() const { return X(17);            }
    cvec<ND> c     () const { return X.tail<ND>();     }
    
    quat qb() const { quat q(qbv()); q.normalize(); return q; }
    quat qc() const { quat q(qcv()); q.normalize(); return q; }

    void qb(const quat& q) { qbv() = q.coeffs(); }
    void qc(const quat& q) { qcv() = q.coeffs(); }

    sat_state propagate(double ti, double tf);

    void set_coe(const coe& coe_in);

    void set_nadir();

    void set_ideal_cam(double f_ideal, const quat& qc_ideal = quat::Identity());

    coe get_coe();

};

class sat_state_err {

    public:

    static const int ND = sat_state::ND;

    static const int N = 6 + ND;

    vec<N> dX;

    double* dx() { return dX.data(); }

    const double* dx() const { return dX.data(); }

    double& dr () { return dX(0); }
    double& dv () { return dX(1); }
    double& dw () { return dX(2); }
    double& dba() { return dX(3); }
    double& dca() { return dX(4); }
    double& df () { return dX(5); }
    rvec<ND> dc () { return dX.tail<ND>(); }
    
    const double& dr () const { return dX(0); }
    const double& dv () const { return dX(1); }
    const double& dw () const { return dX(2); }
    const double& dba() const { return dX(3); }
    const double& dca() const { return dX(4); }
    const double& df () const { return dX(5); }
    cvec<ND> dc () const { return dX.tail<ND>(); }

    static sat_state_err diff(const sat_state& x1, const sat_state& x2);

    static sat_state_err diff(cvec<> x1, cvec<> x2);
    
};

class sat_state_randomizer {

    public:

    double stdr, stdv, stdw, stdba, stdca, stdf, stdc;

    sat_state randomize(const sat_state& state, rando& rand);

    mat<> cov();

};

class sat_dyn : public filter::dyn {
        
    public:

    double stdf, stdt;

    virtual vec<> f(double ti, double tf, cvec<> xi, cvec<> w);

    mat<> cov();

}; 

#endif
