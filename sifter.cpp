#include "sifter.hpp"

#include "angles.hpp"

#include <opencv2/features2d.hpp>

#include <cmath>

using namespace sifter;

points sifter::sift(double t, const sat_state& state, const cv::Mat& image,
        int num_pts) {

    using namespace cv;

	int nOctaveLayers = 5;
	double contrastThreshold = 0.15;
	double edgeThreshold = 10;
	double sigma = 0.5;

    Ptr<SIFT> sifter = SIFT::create(num_pts, nOctaveLayers, contrastThreshold,
            edgeThreshold, sigma);

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

    Ptr<BFMatcher> matcher = BFMatcher::create(NORM_L2, false);

    matcher->knnMatch(query.desc, train.desc, dmatches, train.num_pts);

    matches sm;

    sm.dx = diff.dx;

    sm.num_pts = 0;

    for (const std::vector<DMatch>& dmv : dmatches) {

        for (const DMatch& match : dmv) {
            
            int ind_query = match.queryIdx;
            int ind_train = match.trainIdx;

            sm.query.push_back(query.key_pts[ind_query]);
            sm.train.push_back(train.key_pts[ind_train]);

            sm.dist.push_back(match.distance);

            sm.num_pts++;

        }
    
    }

    return sm;

}
