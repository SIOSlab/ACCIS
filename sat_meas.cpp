#include <sat_meas.hpp>

using namespace sat_meas;

// Measurement with noise w
vec<> gps::h(double t, cvec<> x, cvec<> w) {
    vec<6> xm = w;
    xm.head<3>() += x.segment<3>(ind_r);
    xm.tail<3>() += x.segment<3>(ind_v);
    return xm;    
}
        
// Measurement with randomly generated noise
vec<> gps::H(double t, cvec<> x, rando& rdo) {
    vec<6> w;
    w.head<3>() = std_r * rdo.mvn<3>();
    w.tail<3>() = std_v * rdo.mvn<3>();
    return h(t, x, w);
}

// Measurement noise covariance matrix
mat<> gps::cov() {
    mat<> R = mat<6,6>::Identity();
    R.topLeftCorner<3,3>() *= (std_r * std_r);
    R.bottomRightCorner<3,3>() *= (std_v * std_v);
    return R;
}

// Measurement function for attitude
vec<> star_tracker::h(double t, cvec<> x, cvec<> w) {
    
    quat bfs;
    bfs.setFromTwoVectors(vec<3>::UnitZ(), b); 

    vec<3> n = bfs._transformVector(cos(w(0)) * vec<3>::UnitX() +
                                    sin(w(0)) * vec<3>::UnitY()); 

    Eigen::AngleAxisd qba(w(1), n), qna(w(2), b);

    quat qtru(x.segment<4>(ind_q)), qb(qba), qn(qna);

    quat qmeas = qtru * qn * qb;

    qmeas.normalize();

    return qmeas.coeffs(); 

}

// Generate measurement
vec<> star_tracker::H(double t, cvec<> x, rando& rdo) {
    vec<3> w;
    w(0) = M_PI     * rdo.unif();
    w(1) = std_bor  * rdo.norm();
    w(2) = std_nrm  * rdo.norm();
    return h(t, x, w);
}

// Get noise covariance matrix
mat<> star_tracker::cov() {
    vec<3> d;
    d(0) = M_PI * M_PI * rando::unif_var;
    d(1) = std_bor * std_bor;
    d(2) = std_nrm * std_nrm;
    return d.asDiagonal();    
}


// Noise kurtosis
vec<3> star_tracker::kurt() {
    vec<3> k;
    k(0) = rando::unif_kurt;
    k(1) = 3;
    k(2) = 3;
    return k;
}
