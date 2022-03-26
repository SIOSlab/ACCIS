#ifndef ATTITUDE_PD_HPP
#define ATTITUDE_PD_HPP

#include "mat.hpp"

#include <cmath>

class attitude_pd {

    private:

        quat q_est;
        quat q_ref;

        vec<3> w_est;
        vec<3> w_ref;

        double beta_ref;
        double beta_est;
        
        bool setup; 

    public:

        static int sgn(double x) {
            return (x > 0) - (x < 0);
        }

        static vec<> lpf(cvec<> v_old, cvec<> v_new, double beta) {
            return beta * v_old + (1 - beta) * v_new;
        }

        double kp;
        double kd;

        double dt;

        double tc_ref;
        double tc_est;

        attitude_pd() :
            setup(false)
        {} 

        vec<3> tau(const quat& q, cvec<3> w, const quat& qc, cvec<3> wc) {

            if (setup) {

                q_ref.coeffs() = lpf(q_ref.coeffs(), qc.coeffs(), beta_ref);
                q_est.coeffs() = lpf(q_est.coeffs(), q.coeffs(),  beta_est);

                w_ref = lpf(w_ref, wc, beta_ref);
                w_est = lpf(w_est, w,  beta_est);

            } else {

                q_ref = qc;
                q_est = q;

                w_ref = wc;
                w_est = w;

                beta_ref = exp(-dt/tc_ref);
                beta_est = exp(-dt/tc_est);

                setup = true;

            }

            quat dq = (q_est * q_ref.conjugate()).normalized();
            vec<3> dw = w - wc;

            return - kp * sgn(dq.w()) * dq.vec() - kd * dw;
        
        }

        vec<3> tau(cvec<4>& q, cvec<3> w, cvec<4>& qc, cvec<3> wc) {
            quat qq(q), qqc(qc);
            return tau(qq, w, qqc, wc);
        }

        vec<3> tau(cvec<7> x, cvec<7> xc) {
            return tau(x.head<4>(), x.tail<3>(), xc.head<4>(), xc.tail<3>());
        }

        void reset() {
            setup = false;
        }

};

#endif
