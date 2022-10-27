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

        vec<4> zr;

        mat<> h_mat;

        virtual vec<> h(double t, cvec<> x, cvec<> w);

    };

    //----------------------------------------------------------------

    meas h;

    std::vector<transmission> train; 

    filter::dist dist_w_kp;

    int num_sift_pts;

    double dt_max; 

    double max_dist;

    sat_cam cam;

    filter::dist run(const transmission& query, filter::base& filt);

    static constexpr int M = 20 + img_state_diff::N;

    static constexpr int N = M * M;

    static vec<N> kp_par(cvec<4> zr, const img_state_diff& diff);

};

#endif
