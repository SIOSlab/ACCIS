#include "sifter.hpp"

#include "angles.hpp"

#include <opencv2/features2d.hpp>

#include <cmath>

using namespace sifter;

points sifter::sift(double t, const sat_state& state, const cv::Mat& image,
        int num_pts) {

    using namespace cv;

    Ptr<SIFT> sifter = SIFT::create(num_pts);

    points sp;

    sp.t = t;
    sp.state = state;

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

        sp.key_center.push_back(r);

    }

    return sp;

}

matches sifter::match(const points& query, const points& train,
        double max_dist) {

    using namespace cv;

    img_state_diff diff(query.t, train.t, query.state, train.state);

    std::vector<std::vector<DMatch>> dmatches;

    Ptr<BFMatcher> matcher = BFMatcher::create();

    matcher->knnMatch(query.desc, train.desc, dmatches, 1);

    matches sm;

    sm.dx = diff.dx;

    sm.num_pts = 0;

    for (const std::vector<DMatch>& dmv : dmatches) {

        const double k = 0.6;

        double d1 = dmv[0].distance;
        double d2 = dmv[1].distance;

        if (d1 / d2 < k) {

            int ind_query = dmv[0].queryIdx;
            int ind_train = dmv[0].trainIdx;

            sm.query.push_back(query.key_pts[ind_query]);
            sm.train.push_back(train.key_pts[ind_train]);
            
            sm.num_pts++;

        }
    
    }

    return sm;

}
