#ifndef SAT_MEAS_HPP
#define SAT_MEAS_HPP

#include "angles.hpp"
#include "filter.hpp"
#include "mat.hpp"
#include "rando.hpp"
#include "sat_state.hpp"

#include <cmath>

namespace sat_meas {

    class gps : public filter::meas {

        public:

        // Indices of first component of position & velocity in state vector
        int ind_r;
        int ind_v;

        // Error standard deviations of position (km) & velocity (km/s)
        double std_r;
        double std_v;

        // Measurement with noise w
        virtual vec<> h(double t, cvec<> x, cvec<> w);
        
        // Measurement with randomly generated noise
        vec<> H(double t, cvec<> x, rando& rdo);

        // Measurement noise covariance matrix
        mat<> cov();

        // Constructor
        gps(int ind_r_ = 0, int ind_v_ = 3,
                double std_r_ = 0.004, double std_v_ = 0.02E-3) :
                ind_r(ind_r_), ind_v(ind_v_), std_r(std_r_), std_v(std_v_) {}

    };

    class star_tracker : public filter::meas {

        public:

        // Measurement function for attitude
        virtual vec<> h(double t, cvec<> x, cvec<> w);

        // Generate measurement
        vec<> H(double t, cvec<> x, rando& rdo);

        // Get noise covariance matrix
        mat<> cov(); 

        // Error standard deviation (rad)
        double std_w;

        // Constructor
        star_tracker(double std_w_arcsec = 10) :
            std_w(sec2rad(std_w_arcsec))
        {}

        // Noise kurtosis
        static vec<3> kurt();

    };

    class gyro : public filter::meas {

        public:

        // Indices of first component of angular velocity in state vector
        int ind_w;

        // Error standard deviation (rad/s)
        double std_w;

        // Measurement with noise w
        virtual vec<> h(double t, cvec<> x, cvec<> w);
        
        // Measurement with randomly generated noise
        vec<> H(double t, cvec<> x, rando& rdo);

        // Measurement noise covariance matrix
        mat<> cov();

        // Constructor
        gyro(double std_w_deg_s = 0.001, int ind_w_ = 6) :
            ind_w(ind_w_),
            std_w(deg2rad(std_w_deg_s))
        {}

    };

}

#endif
