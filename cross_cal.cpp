#include <cross_cal.hpp>

#include <angles.hpp>
#include <poly2d.hpp>
#include <sat_state.hpp>

#include <opencv2/features2d.hpp>

#include <cmath>
#include <iostream>

filter::dist cross_cal::run(const transmission& query, filter::base& filt) {

    using namespace cv;
   
    using namespace sifter;

    filter::dist dist_x = query.dist_x;

    meas h;

    h.cam = cam;

    for (const transmission& tr : train) { 
    
        double dt = query.t - tr.t;

        sat_state sq, st;
        sq.X = query.dist_x.mean;
        st.X = tr.dist_x.mean;

        bool overlap = cam.img_overlap(query.t, tr.t, sq, st); 

        if (dt > 0 && dt < dt_max && query.sat_id != tr.sat_id && overlap) {

            matches smatch = match(query.sift, tr.sift, max_dist);

            if (smatch.num_pts > 0) {

                std::cout << "Cross-calibrating Sat " << query.sat_id << " vs. "
                          << tr.sat_id << " with " << smatch.num_pts
                          << " points" << std::endl;

                filter::dist dist_xx = filt.join(dist_x, tr.dist_x);
           
                h.tr = tr.t;

                h.zr = smatch.train;

                vec<> z = poly2d::fit(smatch.train, smatch.query); 

                filter::dist dist_w = dist_w_kp;

                dist_w_kp.cov /= smatch.num_pts;

                dist_xx = filt.update(query.t, z, dist_xx, dist_w, h);

                dist_x = filt.marginal(dist_xx, 0, sat_state::N);

            }
                
        }

    }

    return dist_x;

}

vec<> cross_cal::meas::h(double t, cvec<> x, cvec<> w) {

    sat_state xr, xc;
    xc.X = x.head<sat_state::N>();
    xr.X = x.tail<sat_state::N>();

    return cross_cal_meas(t, tr, xc, xr, cam, zr) + w;

}

vec<> cross_cal_meas(double tc, double tr, const sat_state& xc,
        const sat_state& xr, sat_cam& cam, const std::vector<vec<2>>& zr) {

    std::vector<vec<2>> zc;

    for (int i = 0; i < int(zr.size()); i++)
        zc.push_back(cam.latlon2pix(tc, xc, cam.pix2latlon(tr, xr, zr[i])));

    return poly2d::fit(zr, zc);

}
