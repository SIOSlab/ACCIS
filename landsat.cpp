#include "landsat.hpp"

#include <opencv2/imgcodecs.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define CPPHTTPLIB_OPENSSL_SUPPORT

#include "httplib.h"

cv::Mat landsat::get_image(double latc, double lonc, double side) {

    double lonmin, latmin, lonmax, latmax;
    lonmin = lonc - 0.5 * side;
    latmin = latc - 0.5 * side;
    lonmax = lonc + 0.5 * side;
    latmax = latc + 0.5 * side;

    // TO DO: What if crossing +/-180 longitude?

    std::stringbuf reqbuf;
    std::ostream os(&reqbuf);

    os << "/wms?"
       << "request=GetMap"
       << "&service=WMS"
       << "&version=1.3.0"
       << "&layers=0"
       << "&styles=default"
       << "&crs=CRS:84"
       << "&bbox=" << lonmin << "," << latmin << ","
                   << lonmax << "," << latmax
       << "&width=" << pix
       << "&height=" << pix
       << "&format=image%2Fpng";

    httplib::Client cli("https://landlook.usgs.gov/arcgis/services/"
            "LandsatLook/ImageServer/WMSServer");

    auto res = cli.Get(reqbuf.str().c_str());

    std::cout << reqbuf.str() << std::endl;
    std::cout << res->status << std::endl;
    std::cout << res->body << std::endl;

    std::string body = res->body;
    
    std::vector<char> imgbuf(body.begin(), body.end());

    cv::Mat img = cv::imdecode(imgbuf, cv::IMREAD_COLOR);

    cv::imwrite("img.png", img);

    return img;

}
