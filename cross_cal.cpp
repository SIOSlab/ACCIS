#include <cross_cal.hpp>

#include <angles.hpp>
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

                filter::dist dist_w = filt.join_iid(dist_w_kp, smatch.num_pts);

                h.tr = tr.t;

                h.zr = smatch.train;

                vec<> z(4 * smatch.num_pts);

                for (int i = 0; i < smatch.num_pts; i++)
                    z.segment<4>(4*i) = smatch.query[i];

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

    vec<> z(4 * int(zr.size()));

    for (int i = 0; i < int(zr.size()); i++)
        z.segment<4>(4*i) = cross_cal_meas(t, tr, xc, xr, cam, zr[i]); 

    return z + w;

}

vec<4> cross_cal_meas(double tc, double tr, const sat_state& xc,
        const sat_state& xr, sat_cam& cam, cvec<4> zr) {

    vec<2> pixr1, pixr2;
    pixr1 = zr.head<2>();
    pixr2 = zr.tail<2>(); 

    vec<2> ll1, ll2;
    ll1 = cam.pix2latlon(tr, xr, cam.undistort(xr, pixr1));
    ll2 = cam.pix2latlon(tr, xr, cam.undistort(xr, pixr2));

    vec<2> pixc1, pixc2;
    pixc1 = cam.distort(xc, cam.latlon2pix(tc, xc, ll1));
    pixc2 = cam.distort(xc, cam.latlon2pix(tc, xc, ll2));

    vec<4> zc;
    zc.head<2>() = pixc1;
    zc.tail<2>() = pixc2; 

    return zc;

}
