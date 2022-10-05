#ifndef SIFTER_HPP
#define SIFTER_HPP

#include <opencv2/core.hpp>

#include <vector>

#include "mat.hpp"
#include "sat_cam.hpp"
#include "sat_state.hpp"

namespace sifter {

    struct points {

        double t;

        sat_state state;

        int num_pts;

        std::vector<vec<4>> key_pts;
        std::vector<vec<2>> key_center;

        cv::Mat desc;

    };

    struct matches {

        int num_pts;

        vec<img_state_diff::N> dx;

        std::vector<vec<4>> query;

        std::vector<vec<4>> train;

    };
    
    points sift(double t, const sat_state& state, const cv::Mat& image,
            int num_pts);

    matches match(const points& query, const points& train, sat_cam& cam,
            double max_dist);

}

#endif
