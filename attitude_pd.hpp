#ifndef ATTITUDE_PD_HPP
#define ATTITUDE_PD_HPP

#include "mat.hpp"

class attitude_pd {

    public:

        double kp;
        double kd;

        mat<3,3> J;

        vec<3> tau(const quat& q, cvec<3> w, const quat& qc, cvec<3> wc) const {
            quat dq = q * qc.conjugate();
            vec<3> W = dq._transformVector(wc);
            return W.cross(J*W) - kp*dq.vec() - kd*(w - W);
        }

};

#endif
