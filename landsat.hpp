#ifndef LANDSAT_HPP
#define LANDSAT_HPP

#include <opencv2/core>

namespace landsat {

    cv::Mat get_image(double latc, double lonc, double side, int pix);

}

#endif
