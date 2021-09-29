#ifndef ATTITUDE_SMC_HPP
#define ATTITUDE_SMC_HPP

#include "mat.hpp"

class attitude_smc {

    public:

    mat<3,3> J;
    mat<3,3> G;
    double eps;
    double k;

    static int sign(double s) {
        return (s > 0) - (s < 0);
    }

    double sat(double s) const {
        return s > eps ? 1 : (s < eps ? -1 : (s / eps));
    }

    vec<3> sat(cvec<3> s) const {
        vec<3> y;
        for (int i = 0; i < 3; i++)
            y(i) = sat(s(i));
        return y;
    }

    vec<3> control_torque(const quat& qc, const quat& q, cvec<3> wc, cvec<3> w,
            cvec<3> wcd) {
        quat dq = q * qc.conjugate(); 
        int sgn_dq4 = sign(dq.w());
        vec<3> sb = sat(w - wc + k * sgn_dq4 * dq.vec());
        return J * ((k/2)*(fabs(dq.w())*(wc - w)
                    - sgn_dq4*dq.vec().cross(w + wc)) + wcd - G*sb)
                    + w.cross(J*w); 
    }

};

#endif
