#ifndef ATTITUDE_PD_HPP
#define ATTITUDE_PD_HPP

#include "mat.hpp"

class attitude_pd {

    public:

        double kp;
        double kd;

        mat<3,3> J;

        vec<3> tau(const quat& q, cvec<3> w, const quat& qc, cvec<3> wc) const {
            quat dq = q.normalized() * qc.normalized().conjugate();
            vec<3> W = dq._transformVector(wc);
            return W.cross(J*W) - kp*dq.vec() - kd*(w - W);
        }

        vec<3> tau(cvec<4>& q, cvec<3> w, cvec<4>& qc, cvec<3> wc) const {
            quat qq(q), qqc(qc);
            return tau(qq, w, qqc, wc);
        }

        vec<3> tau(cvec<7> x, cvec<7> xc) const {
            return tau(x.head<4>(), x.tail<3>(), xc.head<4>(), xc.tail<3>());
        }

};

#endif
