#include "sifter.hpp"

#include "angles.hpp"

#include <opencv2/features2d.hpp>

#include <cmath>

using namespace sifter;

points sifter::sift(const cv::Mat& image, int num_pts) {

    using namespace cv;

    Ptr<SIFT> sifter = SIFT::create(num_pts);

    points sp;

    sp.num_pts = num_pts;

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

matches sifter::match(const points& query, const points& train, double dmax) {

    using namespace cv;

    std::vector<std::vector<DMatch>> dmatches;

    Ptr<BFMatcher> matcher = BFMatcher::create();

    matcher->knnMatch(query.desc, train.desc, dmatches, 1);

    matches sm;

    sm.num_pts = 0;

    for (const std::vector<DMatch>& dmv : dmatches) {
        DMatch dmatch = dmv[0];
        if (dmatch.distance < dmax) {
            sm.query.push_back(query.key_pts[dmatch.queryIdx]);
            sm.train.push_back(train.key_pts[dmatch.trainIdx]);
            sm.num_pts++;
        }
    }

    return sm;

}
