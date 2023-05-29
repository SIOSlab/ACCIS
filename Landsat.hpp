#ifndef LANDSAT_HPP
#define LANDSAT_HPP

#include <opencv2/core.hpp>

namespace Landsat {

    cv::Mat image(int widp, int lenp, double latc, double lonc, double dp);

    cv::Mat image(int widd, int lend, int latc, int lonc);

}

#endif
