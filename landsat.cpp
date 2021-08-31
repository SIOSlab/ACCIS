#include "landsat.hpp"

#include "httplib.h"

#include <opencv2/imgcodecs.hpp>

#include <sstream>
#include <string>
#include <vector>

cv::Mat landsat::get_image(double latc, double lonc, double side, int pix) {

    double lonmin, latmin, lonmax, latmax;
    lonmin = lonc - 0.5 * side;
    latmin = latc - 0.5 * side;
    lonmax = lonc + 0.5 * side;
    latmax = latc + 0.5 * side;

    // TO DO: What if crossing +/-180 longitude?

    std::stringbuf urlbuf;

    urlbuf << "https://worldwind25.arc.nasa.gov/wms?"
           << "request=GetMap"
           << "&service=WMS"
           << "&version=1.3.0"
           << "&layers=BlueMarble-200409,ESAT"
           << "&styles=default"
           << "&crs=CRS:84"
           << "&bbox=" << lonmin << "," << latmin << "," <<
                       << lonmax << "," << latmax
           << "&width=" << pix
           << "&height=" << pix
           << "&format=image%2Fpng";

    httplib::Client cli(urlbuf.str());

    auto res = cli.get();

    std::string body = res->body;

    std::vector<char> imgbuf(body.begin(), body.end());

    return cv::imdecode(imgbuf, IMREAD_COLOR);

}
