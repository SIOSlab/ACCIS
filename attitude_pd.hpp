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
            
            mat<3,3> dA = q.toRotationMatrix()
                * qc.toRotationMatrix().transpose();
            
            vec<3> dw = w - dA * wc;
            
            return (dA * wc).cross(J * dA * wc) - kp * dq.vec() - kd * dw;
        
        }

};

#endif
