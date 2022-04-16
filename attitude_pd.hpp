#ifndef ATTITUDE_PD_HPP
#define ATTITUDE_PD_HPP

#include "mat.hpp"

class attitude_pd {

    public:

        double kp;
        double kd;

        mat<3,3> J;

        quat q0;
        vec<3> wc;

        static int sgn(double x) {
            return (x > 0) - (x < 0);
        }

        quat qct(double t) const {

            Eigen::AngleAxisd aa(wc.norm()*t, wc.normalized()); 

            quat qc = q0 * aa;

            return qc;

        }

        vec<3> tau(const quat& q, cvec<3> w, double t) const {
           
            quat qc = qct(t);

            quat dq = q * qc.conjugate();

            return - q.inverse()._transformVector(kp * sgn(dq.w()) * dq.vec()
                    + kd * (q._transformVector(w) - qc._transformVector(wc)));
        
        }

};

#endif
