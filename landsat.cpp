#include "landsat.hpp"

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"

#include <opencv2/imgcodecs.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

cv::Mat landsat::get_image(double latc, double lonc, double side) {

    double lonmin, latmin, lonmax, latmax;
    lonmin = lonc - 0.5 * side;
    latmin = latc - 0.5 * side;
    lonmax = lonc + 0.5 * side;
    latmax = latc + 0.5 * side;

    // TO DO: What if crossing +/-180 longitude?

    std::stringbuf reqbuf;
    std::ostream os(&reqbuf);

    os << "https://worldwind25.arc.nasa.gov"
       << "/wms?"
       << "request=GetMap"
       << "&service=WMS"
       << "&version=1.3.0"
       << "&layers=BlueMarble-200409,ESAT"
       << "&styles=default"
       << "&crs=CRS:84"
       << "&bbox=" << lonmin << "," << latmin << ","
                   << lonmax << "," << latmax
       << "&width=" << pix
       << "&height=" << pix
       << "&format=image%2Fpng";

    httplib::Client cli("https://worldwind25.arc.nasa.gov");

    auto res = cli.Get(reqbuf.str().c_str());

    std::string body = res->body;

    std::cout << reqbuf.str() << std::endl;
    std::cout << res->status << std::endl;
    std::cout << res->body << std::endl;

    std::vector<char> imgbuf(body.begin(), body.end());

    cv::Mat img = cv::imdecode(imgbuf, cv::IMREAD_COLOR);

    cv::imwrite("img.png", img);

    return img;

}
