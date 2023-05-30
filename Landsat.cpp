#include "Landsat.hpp"

#include "format.hpp"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <cmath>
#include <string>
#include <vector>

cv::Mat Landsat::image(int widp, int lenp, double latc, double lonc,
        double dp) {

    const int pix_deg = 1000;

    int ilatc = int(ceil (latc));
    int ilonc = int(floor(lonc));

    int widd = int(ceil(widp * dp));
    int lend = int(ceil(lenp * dp));

    cv::Point2f src[4], dst[4];

    dst[0].x = 0;
    dst[0].y = 0;

    dst[1].x = widp;
    dst[1].y = 0;

    dst[2].x = widp;
    dst[2].y = lenp;

    dst[3].x = 0;
    dst[3].y = lenp;

    src[0].x = float(pix_deg * (lonc - ilonc));
    src[0].y = float(pix_deg * (ilatc - latc));

    src[1].x = src[0].x + float(pix_deg * dp * widp);
    src[1].y = src[0].y;

    src[2].x = src[1].x;
    src[2].y = src[1].y + float(pix_deg * dp * lenp);

    src[3].x = src[0].x;
    src[3].y = src[2].y; 

    cv::Mat iimg = image(widd, lend, ilatc, ilonc);

    cv::Mat T = getPerspectiveTransform(src, dst);

    cv::Size imsize(widp, lenp);

    cv::Mat img(imsize, iimg.type());

    cv::warpPerspective(iimg, img, T, imsize, cv::INTER_LINEAR);
    
    return img;


}
    
cv::Mat Landsat::image(int widd, int lend, int latc, int lonc) {

    std::vector<cv::Mat> rows;

    for (int row = 0; row < lend; row++) {

        std::vector<cv::Mat> cols;

        int clat = (90 - latc) + row;

        for (int col = 0; col < widd; col++) {

            int lon = (lonc + col + 360) % 360;

            std::string filename = "mosaic/tile_" + int2str0(clat, 3) + "_" +
                                                    int2str0(lon,  3) + ".png";

            cols.push_back(cv::imread(filename)); 

        }

        cv::Mat img_row;

        cv::hconcat(cols, img_row);

        rows.push_back(img_row);

    }

    cv::Mat img;

    cv::vconcat(rows, img);

    return img;

}
