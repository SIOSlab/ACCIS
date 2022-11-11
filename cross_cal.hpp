#ifndef CROSS_CALIBRATION_HPP
#define CROSS_CALIBRATION_HPP

#include <filter.hpp>
#include <mat.hpp>
#include <sat_cam.hpp>
#include <sat_state.hpp>
#include <sifter.hpp>

#include <opencv2/core.hpp>

#include <vector>

class cross_cal {

    public:

    struct transmission {

        int sat_id;

        int step;

        double t;

        sifter::points sift;

        filter::dist dist_x;

    };

    class meas : public filter::meas {

        public:

        double tr;

        std::vector<vec<2>> zr;

        sat_cam cam;

        virtual vec<> h(double t, cvec<> x, cvec<> w);

    };

    //----------------------------------------------------------------

    std::vector<transmission> train; 

    filter::dist dist_w_kp;

    int num_sift_pts;

    double dt_max; 

    double max_dist;

    sat_cam cam;

    filter::dist run(const transmission& query, filter::base& filt);

};

vec<> cross_cal_meas(double tc, double tr, const sat_state& xc,
        const sat_state& xr, sat_cam& cam, const std::vector<vec<2>>& zr);

#endif
