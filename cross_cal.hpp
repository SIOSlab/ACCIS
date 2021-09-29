#ifndef CROSS_CALIBRATION_HPP
#define CROSS_CALIBRATION_HPP

#include <filter.hpp>
#include <mat.hpp>
#include <sat_cam.hpp>
#include <sat_state.hpp>

#include <opencv2/core.hpp>

#include <vector>

class cross_cal {

    public:

    struct sift_pts {

        int num_pts;

        std::vector<vec<4>> key_pts;

        cv::Mat desc;

    };

    struct sift_match {

        int num_pts;

        std::vector<vec<4>> query;

        std::vector<vec<4>> train;

    };

    struct transmission {

        int sat_id;

        int step;

        double t;

        sift_pts sift;

        filter::dist dist_x;

    };

    class meas : public filter::meas {

        public:

        double tr;

        vec<4> zr;

        sat_cam cam;

        virtual vec<> h(double t, cvec<> x, cvec<> w);

    };

    sift_pts sift(const cv::Mat& image); 

    sift_match match(const transmission& query, const transmission& train); 

    //----------------------------------------------------------------

    std::vector<transmission> train; 

    filter::dist dist_w_kp;

    int num_sift_pts;

    double dt_max; 

    double kp_r_max;

    double kp_d_max;

    sat_cam cam;

    filter::dist run(const transmission& query, filter::base& filt);

};

#endif
