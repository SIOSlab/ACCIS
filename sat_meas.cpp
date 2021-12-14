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

    /*    
    quat bfs;
    bfs.setFromTwoVectors(vec<3>::UnitZ(), b); 

    vec<3> n = bfs._transformVector(cos(w(0)) * vec<3>::UnitX() +
                                    sin(w(0)) * vec<3>::UnitY()); 

    Eigen::AngleAxisd qba(w(1), n), qna(w(2), b);
    quat qb(qba), qn(qna);

    sat_state s;
    s.X = x;
    quat qtru = s.qb(); 
    s.qb(qtru * qn * qb);

    return s.pb(); 
    */

    Eigen::AngleAxisd err(w.norm(), w.normalized());
    quat qerr(err);

    sat_state s;
    s.X = x;
    quat qtru = s.qb(); 
    s.qb(qerr * qtru);

    return s.pb(); 

}

// Generate measurement
vec<> star_tracker::H(double t, cvec<> x, rando& rdo) {
    return h(t, x, rdo.mvn<3>() * std_w); 
}

// Get noise covariance matrix
mat<> star_tracker::cov() {
    return mat<3,3>::Identity() * std_w * std_w;
}


// Noise kurtosis
vec<3> star_tracker::kurt() {
    vec<3> k;
    k.setConstant(3);
    return k;
}

// Measurement with noise w
vec<> gyro::h(double t, cvec<> x, cvec<> w) {
    return x.segment<3>(ind_w) + w;
}
        
// Measurement with randomly generated noise
vec<> gyro::H(double t, cvec<> x, rando& rdo) {
    return h(t, x, rdo.mvn<3>() * std_w); 
}

// Measurement noise covariance matrix
mat<> gyro::cov() {
    return std_w * std_w * mat<3,3>::Identity();
}
