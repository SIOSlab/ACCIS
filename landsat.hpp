#ifndef LANDSAT_HPP
#define LANDSAT_HPP

#include <opencv2/core.hpp>

namespace landsat {

    const int pix = 1000;

    cv::Mat get_image(double latc, double lonc, double side);

}

#endif
