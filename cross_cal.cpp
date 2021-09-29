#include <cross_cal.hpp>

#include <angles.hpp>
#include <sat_state.hpp>

#include <opencv2/features2d.hpp>

#include <cmath>
#include <iostream>

filter::dist cross_cal::run(const transmission& query, filter::base& filt) {

    using namespace cv;
    
    filter::dist dist_x = query.dist_x;

    filter::dist dist_w_2kp = filt.join(dist_w_kp, dist_w_kp);

    filter::dist dist_w_4kp = filt.join(dist_w_2kp, dist_w_2kp);

    meas h;

    h.cam = cam;

    for (const transmission& tr : train) { 
    
        double dt = query.t - tr.t;
    
        if (dt > 0 && dt < dt_max && query.sat_id != tr.sat_id) {

            sift_match smatch = match(query, tr);

            filter::dist dist_w = filt.join(dist_w_4kp, tr.dist_x);
            
            h.tr = tr.t;

            for (int i = 0; i < smatch.num_pts; i++) {

                h.zr = smatch.train[i];

                dist_x = filt.update(query.t, smatch.query[i],
                        dist_x, dist_w, h);

            }
                
        }

    }

    return dist_x;

}

cross_cal::sift_pts cross_cal::sift(const cv::Mat& image) {

    using namespace cv;

    Ptr<SIFT> sifter = SIFT::create(num_sift_pts);

    sift_pts sp;

    sp.num_pts = num_sift_pts;

    std::vector<KeyPoint> kps;

    sifter->detectAndCompute(image, noArray(), kps, sp.desc);

    for (const KeyPoint& kp : kps) {
        
        vec<2> r, dr;

        r.x() = kp.pt.x;
        r.y() = kp.pt.y;

        dr.x() = 0.5 * kp.size * cos(deg2rad(kp.angle));
        dr.y() = 0.5 * kp.size * sin(deg2rad(kp.angle));
        
        vec<4> z;
        
        z.head<2>() = r + dr;
        z.tail<2>() = r - dr;
        
        sp.key_pts.push_back(z);

    }

    return sp;

}

cross_cal::sift_match cross_cal::match(const cross_cal::transmission& query,
        const cross_cal::transmission& train) {
    
    using namespace cv;

    std::vector<std::vector<DMatch>> dmatches;

    Ptr<BFMatcher> matcher = BFMatcher::create();

    matcher->knnMatch(query.sift.desc, train.sift.desc, dmatches, 1);

    std::cout << "Number of keypoint matches: " << dmatches.size() << std::endl;

    sift_match smatch;

    smatch.num_pts = 0;

    sat_state x1, x2;
    x1.X = query.dist_x.mean;
    x2.X = train.dist_x.mean;

    for (const std::vector<DMatch>& dmv : dmatches) {
        DMatch dmatch = dmv[0];
        vec<2> pix1 = query.sift.key_pts[dmatch.queryIdx].head<2>();
        vec<2> pix2 = query.sift.key_pts[dmatch.trainIdx].head<2>();
        double pt_dist = cam.pt_dist(query.t, train.t, x1, x2, pix1, pix2);
        if (dmatch.distance < kp_r_max && pt_dist < kp_d_max) {
            std::cout << "r = " << dmatch.distance << std::endl;
            std::cout << "d = " << pt_dist << std::endl;
            smatch.query.push_back(query.sift.key_pts[dmatch.queryIdx]);
            smatch.train.push_back(train.sift.key_pts[dmatch.trainIdx]);
            smatch.num_pts++;
        }
    }

    return smatch;

}

vec<> cross_cal::meas::h(double t, cvec<> x, cvec<> w) {

    sat_state xr, xc;
    xr.X = w.tail<sat_state::N>();
    xc.X = x;

    vec<2> wr1, wr2, wc1, wc2;
    wr1 = w.segment<2>(0);
    wr2 = w.segment<2>(2);
    wc1 = w.segment<2>(4);
    wc2 = w.segment<2>(6);

    vec<2> pixr1, pixr2;
    pixr1 = zr.head<2>() - wr1;
    pixr2 = zr.tail<2>() - wr2; 

    vec<2> ll1, ll2;
    ll1 = cam.pix2latlon(tr, xr, cam.undistort(xr, pixr1));
    ll2 = cam.pix2latlon(tr, xr, cam.undistort(xr, pixr2));

    vec<2> pixc1, pixc2;
    pixc1 = cam.distort(xc, cam.latlon2pix(t, xc, ll1));
    pixc2 = cam.distort(xc, cam.latlon2pix(t, xc, ll2));

    vec<4> zc;
    zc.head<2>() = pixc1 + wc1;
    zc.tail<2>() = pixc2 + wc2;

    return zc;

}
    
