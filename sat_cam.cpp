#include <sat_cam.hpp>

#include <angles.hpp>
#include <landsat.hpp>
#include <polynomial.hpp>
#include <roam.h>

#include <opencv2/imgproc.hpp>

#include <cmath>
#include <iostream>

vec<2> sat_cam::center() {
    vec<2> c;
    c.x() = 0.5 * widp;
    c.y() = 0.5 * lenp;
    return c;
}

double sat_cam::radpx() {
    return 0.5 * hypot(widp, lenp);
}

vec<2> sat_cam::distort(const sat_state& x, cvec<2> pix) {
    double r, s;
    vec<2> pixc = pix - center();
    vec<sat_state::ND+1> c;
    c(0) = 1 - x.c().sum();
    c.tail<sat_state::ND>() = x.c();
    r = pixc.norm() / radpx();
    s = polynomial::eval(x.c(), r);
    return center() + s * pixc; 
}

vec<2> sat_cam::undistort(const sat_state& x, cvec<2> pix) {
    static constexpr double tol = 1E-8;
    double r, rd;
    vec<2> pixc = pix - center();
    rd = pixc.norm() / radpx();
    vec<sat_state::ND+2> k;
    k(0) = -rd;
    k(1) = 1 - x.c().sum();
    k.tail<sat_state::ND>() = x.c();
    r = polynomial::solve(k, rd, tol);
    return center() + (r / rd) * pixc;  
}

vec<2> sat_cam::pix2latlon(double t, const sat_state& x, cvec<2> pix) {
    vec<3> los_cam, los_eci;
    los_cam.head<2>() = pix / rho;
    los_cam.z() = u;
    los_eci = cam2eci(x, los_cam);
    static const double tol = 1E-6;
    double lat, lon, dist;
    losgeo_(&t, x.r().data(), los_eci.data(), &tol, &lat, &lon, &dist);
    vec<2> latlon;
    latlon.x() = lat;
    latlon.y() = lon;
    return latlon;
} 

vec<2> sat_cam::latlon2pix(double t, const sat_state& x, cvec<2> latlon) {
    double lat, lon, alt;
    lat = latlon.x();
    lon = latlon.y();
    alt = 0;
    vec<3> r;
    ll2eci_(&t, &lat, &lon, &alt, r.data());
    vec<3> r_cam = eci2cam(x, r - x.r());
    return r_cam.head<2>() * (u / r_cam.z()) + center();
}

mat<2,4> sat_cam::corner_pix() {
    mat<2,4> pix;
    pix << 0, widp, widp,    0,
           0,    0, lenp, lenp;
    return pix;
}

double sat_cam::pt_dist(double t1, double t2, const sat_state& x1,
        const sat_state& x2, cvec<2> pix1, cvec<2> pix2) {
    vec<2> ll1 = pix2latlon(t1, x1, pix1);
    vec<2> ll2 = pix2latlon(t2, x2, pix2);
    return hav_dist(ll1.x(), ll2.x(), ll1.y(), ll2.y());
}

mat<2,4> sat_cam::corner_latlon(double t, const sat_state& x) {
    mat<2,4> pix, latlon;
    pix = corner_pix();
    for (int i = 0; i < 4; i++)
        latlon.col(i) = pix2latlon(t, x, pix.col(i));
    return latlon;
}    

vec<3> sat_cam::cam2eci(const sat_state& x, cvec<3> v) {
    return (x.qb() * x.qc())._transformVector(v);
}

vec<3> sat_cam::eci2cam(const sat_state& x, cvec<3> v) {
    return (x.qb() * x.qc()).inverse()._transformVector(v);
}

double sat_cam::sig_defocus(double t, const sat_state& x) {
    vec<3> los = cam2eci(x, vec<3>::UnitZ());
    static const double tol = 1E3;
    double lat, lon, dist;
    losgeo_(&t, x.r().data(), los.data(), &tol, &lat, &lon, &dist);
    return 0.5*rho*dist*A * fabs(1/x.f() - 1/u - 1E-6/dist);
}

cv::Mat sat_cam::defocus(const cv::Mat& imo, double t, const sat_state& x) {

    using namespace cv;

    Mat imd(imo.size(), imo.type());

    Size ksize(0, 0);

    GaussianBlur(imo, imd, ksize, sig_defocus(t, x));

    return imd;

}    

cv::Mat sat_cam::distort(const cv::Mat& imo, const sat_state& x) {

    using namespace cv;

    // Image mapping values
    Mat xmap(imo.size(), CV_32FC1), ymap(imo.size(), CV_32FC1);
    vec<2> r, rd;
    for (int i = 0; i < xmap.rows; i++) {
        for (int j = 0; j < xmap.cols; j++) {
            rd.x() = i;
            rd.y() = j;
            r = undistort(x, rd);
            xmap.at<float>(i,j) = float(r.x());
            ymap.at<float>(i,j) = float(r.y());
        }
    }

    // Map image
    Mat imd(imo.size(), imo.type());
    remap(imo, imd, xmap, ymap, INTER_LINEAR);

    return imd;

}


cv::Mat sat_cam::ideal_image(cmat<2,4> latlon_corners) {

    using namespace cv;

    double latc, lonc, side;
    latc = latlon_corners.row(0).mean();
    lonc = latlon_corners.row(1).mean();
    side = (latlon_corners.rowwise().maxCoeff()
           - latlon_corners.rowwise().minCoeff()).maxCoeff();

    Mat sector = landsat::get_image(latc, lonc, side);

    Point2f src[4], dst[4];

    dst[0].x = 0;
    dst[0].y = 0;

    dst[1].x = widp;
    dst[1].y = 0;
    
    dst[2].x = widp;
    dst[2].y = lenp;

    dst[3].x = 0;
    dst[3].y = lenp;

    for (int i = 0; i < 4; i++) {
        src[i].x = landsat::pix
            * float(0.5 + (latlon_corners(1,i) - lonc) / side);
        src[i].y = landsat::pix
            * float(0.5 + (latlon_corners(0,i) - latc) / side);
    }

    Mat T = getPerspectiveTransform(src, dst);

    Size imsize(widp, lenp);

    Mat img(imsize, sector.type());

    warpPerspective(sector, img, T, imsize, INTER_LINEAR);
    
    return img;

}

cv::Mat sat_cam::real_image(double t, const sat_state& x) {

    mat<2,4> latlon_corners = corner_latlon(t, x); 

    std::cout << "Acquiring image" << std::endl;
    std::cout << latlon_corners << std::endl;

    cv::Mat ideal_img = ideal_image(latlon_corners);

    std::cout << "Image acquired" << std::endl;

    cv::Mat distorted_img = distort(ideal_img, x);

    cv::Mat defocused_img = defocus(distorted_img, t, x); 

    return defocused_img;

}
