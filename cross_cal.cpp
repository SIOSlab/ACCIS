#include <cross_cal.hpp>

#include <angles.hpp>
#include <sat_state.hpp>

#include <opencv2/features2d.hpp>

#include <cmath>
#include <complex>
#include <iostream>

filter::dist cross_cal::run(const transmission& query, filter::base& filt) {

    using namespace cv;
   
    using namespace sifter;

    filter::dist dist_x = query.dist_x;

    for (const transmission& tr : train) { 
    
        double dt = fabs(query.t - tr.t);

        sat_state sq, st;
        sq.X = query.dist_x.mean;
        st.X = tr.dist_x.mean;

        bool overlap = cam.img_overlap(query.t, tr.t, sq, st); 

        if (dt < dt_max && query.sat_id != tr.sat_id && overlap) {

            matches smatch = match(query.sift, tr.sift, max_dist);

            if (smatch.num_pts > 0) {

                std::cout << "Cross-calibrating Sat " << query.sat_id << " vs. "
                          << tr.sat_id << " with " << smatch.num_pts
                          << " points" << std::endl;

                filter::dist dist_xx = filt.join(dist_x, tr.dist_x);
           
                h.tr = tr.t;

                mat<4> z_diff(4, smatch.num_pts);
                for (int i = 0; i < smatch.num_pts; i++)
                    z_diff.col(i) = kp_diff(smatch.query[i], smatch.train[i]); 

                vec<4> z = z_diff.rowwise().mean();

                filter::dist dist_w = dist_w_kp;

                dist_w.cov /= smatch.num_pts; 

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

    img_state_diff diff(t, tr, xc, xr);

    return h_mat * diff.dx + w;

}

vec<4> cross_cal::kp_diff(cvec<4> z, cvec<4> zr) {

    std::complex<double> z1(z.x(), z.y());
    std::complex<double> z2(z.z(), z.w());

    std::complex<double> zr1(zr.x(), zr.y());
    std::complex<double> zr2(zr.z(), zr.w());

    std::complex<double> a = (z2 - z1) / (zr2 - zr1);

    std::complex<double> b = z1 - a * zr1;

    vec<4> d;

    d.x() = a.real();
    d.y() = a.imag();
    d.z() = b.real();
    d.w() = b.imag();

    return d;

}
