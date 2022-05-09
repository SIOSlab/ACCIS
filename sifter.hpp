#ifndef SIFTER_HPP
#define SIFTER_HPP

#include <opencv2/core.hpp>

#include <vector>

#include "mat.hpp"

namespace sifter {

    struct points {

        int num_pts;

        std::vector<vec<4>> key_pts;

        cv::Mat desc;

    };

    struct matches {

        int num_pts;

        std::vector<vec<4>> query;

        std::vector<vec<4>> train;

        std::vector<double> dist;

    };
    
    points sift(const cv::Mat& image, int num_pts); 

    matches match(const points& query, const points& train);

}

#endif
