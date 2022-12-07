#ifndef SAT_CAM_HPP
#define SAT_CAM_HPP

#include <mat.hpp>
#include <sat_state.hpp>

#include <opencv2/core.hpp>

class sat_cam {

    public:

    //--------------------------------------------------------------------------
    //  Camera Parameters
    //--------------------------------------------------------------------------

    // Image width & length (pixels)
    int widp, lenp;

    // Pixels per millimeter
    double rho;
    
    // Distance from mirror plane to image plane (mm) 
    double u;

    // Aperture (mm)
    double A;
    
    //--------------------------------------------------------------------------
    //  Transformations
    //--------------------------------------------------------------------------

    // Image center (pixels)
    vec<2> center();

    // Maximum radius of image (pixels)
    double radpx();

    // Distort pixel
    vec<2> distort(const sat_state& x, cvec<2> pix);

    // Undistort pixel
    vec<2> undistort(const sat_state& x, cvec<2> pix);

    // Transform vector from camera frame to ECI
    static vec<3> cam2eci(const sat_state& x, cvec<3> v);

    // Transform vector from ECI to camera frame
    static vec<3> eci2cam(const sat_state& x, cvec<3> v);

    // Pixels to Latitude & Longitude (deg) -- No distortion
    vec<2> pix2latlon(double t, const sat_state& x, cvec<2> pix);    

    // Latitude & Longitude (deg) to pixels -- No distortion
    vec<2> latlon2pix(double t, const sat_state& x, cvec<2> latlon);

    // Image corners (pixels)
    mat<2,4> corner_pix();

    // Latitude & Longitude (deg) of image corners -- No distortion
    mat<2,4> corner_latlon(double t, const sat_state& x);    

    // Great circle distance (deg) between two image points
    double pt_dist(double t1, double t2, const sat_state& x1,
            const sat_state& x2, cvec<2> pix1, cvec<2> pix2);

    // Radius of small circle (deg) inscribed in image
    double img_rad(double t, const sat_state& x);

    // Determine whether two images overlap
    bool img_overlap(double t1, double t2, const sat_state& x1,
            const sat_state& x2);

    // Determine whether a point is in the overlapping region of two images
    bool pt_overlap(double t1, double t2, const sat_state& x1,
            const sat_state& x2, cvec<2> pt1);

    //--------------------------------------------------------------------------
    //  Image Distortions
    //--------------------------------------------------------------------------

    // Standard deviation of blur for defocus (pixels)
    double sig_defocus(double t, const sat_state& x);

    // Defocus
    cv::Mat defocus(const cv::Mat& imo, double t, const sat_state& x);    

    // Radial distortion
    cv::Mat distort(const cv::Mat& imo, const sat_state& x);

    //--------------------------------------------------------------------------
    //  Image Generation
    //--------------------------------------------------------------------------

    // Ideal image from latitude & longitude (deg) of corners
    cv::Mat ideal_image(cmat<2,4> latlon_corners);

    // "Real" image from satellite state
    cv::Mat real_image(double t, const sat_state& x);

    // Percentage of black pixels in image
    static double blp(const cv::Mat& img);

};

#endif
