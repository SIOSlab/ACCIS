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

    std::vector<KeyPoint> kps;

    sifter->detectAndCompute(image, noArray(), kps, sp.desc);

    sp.num_pts = kps.size();

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
        sat_cam& cam, double max_dist) {

    using namespace cv;

    std::vector<bool> mask_query(query.num_pts);
    std::vector<bool> mask_train(train.num_pts);

    for (int i = 0; i < query.num_pts; i++)
        mask_query.at(i) = cam.pt_overlap(query.t, train.t,
               query.state, train.state, query.key_center.at(i));

    for (int i = 0; i < train.num_pts; i++)
        mask_train.at(i) = cam.pt_overlap(train.t, query.t,
               train.state, query.state, train.key_center.at(i));

    Mat mask(query.num_pts, train.num_pts, CV_8UC1);

    for (int i = 0; i < query.num_pts; i++)
        for (int j = 0; j < train.num_pts; j++)
            mask.at<uchar>(i, j) = mask_query.at(i) && mask_train.at(j);

    img_state_diff diff(query.t, train.t, query.state, train.state);

    std::vector<std::vector<DMatch>> dmatches;

    Ptr<BFMatcher> matcher = BFMatcher::create();

    matcher->knnMatch(query.desc, train.desc, dmatches, 2, mask, true);

    matches sm;

    sm.dx = diff.dx;

    sm.num_pts = 0;

    for (const std::vector<DMatch>& dmv : dmatches) {

        if (dmv.size() >= 2) {

            double d1 = dmv.at(0).distance;
            double d2 = dmv.at(1).distance;

            if (d1 / d2 < max_dist) {

                int ind_query = dmv.at(0).queryIdx;
                int ind_train = dmv.at(0).trainIdx;

                sm.query.push_back(query.key_pts[ind_query]);
                sm.train.push_back(train.key_pts[ind_train]);
            
                sm.num_pts++;

            }

        }
    
    }

    return sm;

}
