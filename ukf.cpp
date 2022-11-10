#include <ukf.hpp>

#include <cmath>

using namespace filter;

// Filter prediction
dist ukf::predict(double ti, double tf, const dist& distXi, const dist& distW,
        dyn& f) {

    sig S(distXi, distW, K);

    mat<> Xf(distXi.dim, S.n);

    for (int i = 0; i < S.n; i++)
        Xf.col(i) = f.f(ti, tf, S.X.col(i), S.W.col(i));

    dist distXf(distXi.dim);

    distXf.mean = Xf * S.w;

    distXf.cov = Xf * S.w.asDiagonal() * Xf.transpose()
        - distXf.mean * distXf.mean.transpose();

    return distXf; 

}

// Filter update
dist ukf::update(double t, cvec<> z, const dist& distXp, const dist& distW,
        meas& h) {

    using namespace Eigen;

    sig S(distXp, distW, K);

    mat<> Z(z.size(), S.n);

    for (int i = 0; i < S.n; i++)
        Z.col(i) = h.h(t, S.X.col(i), S.W.col(i));

    vec<> zm = Z * S.w;

    mat<> Pzx = Z * S.w.asDiagonal() * S.X.transpose()
                    - zm * distXp.mean.transpose();

    mat<> Pzz = Z * S.w.asDiagonal() * Z.transpose() - zm * zm.transpose();

    BDCSVD<mat<>> Pzz_svd(Pzz, ComputeFullU | ComputeFullV);

    mat<> Kt = Pzz_svd.solve(Pzx);

    dist distXu = distXp;

    distXu.mean += Kt.transpose() * (z - zm);

    distXu.cov -= Pzx.transpose() * Kt;

    bool upd_ok = !distXu.mean.hasNaN() && !distXu.cov.hasNaN();

    return upd_ok ? distXu : distXp;

}

// Joint distribution from two independent distributions 
dist ukf::join(const dist& dist1, const dist& dist2) {

    dist distc(dist1.dim + dist2.dim);

    distc.mean.head(dist1.dim) = dist1.mean;
    distc.mean.tail(dist2.dim) = dist2.mean;

    distc.cov.setZero();

    distc.cov.topLeftCorner(dist1.dim, dist1.dim) = dist1.cov;
    distc.cov.bottomRightCorner(dist2.dim, dist2.dim) = dist2.cov;

    return distc;

}

// Joint distribution from n i.i.d. variables
dist ukf::join_iid(const dist& dist_i, int n) {

    int m = dist_i.dim;

    dist dist_j(m * n);

    dist_j.mean = dist_i.mean.replicate(n, 1);

    dist_j.cov.setZero();

    for (int i = 0; i < n; i++)
        dist_j.cov.block(m*i, m*i, m, m) = dist_i.cov;

    return dist_j;

}

// Marginal distribution for distribution components
dist ukf::marginal(const dist& joint_dist, int ind, int dim) {

    dist marg_dist(dim);

    marg_dist.mean = joint_dist.mean.segment(ind, dim);

    marg_dist.cov = joint_dist.cov.block(ind, ind, dim, dim);

    return marg_dist;

} 

// Matrix square root
mat<> ukf::mat_sqrt(cmat<> A) {
    using namespace Eigen;
    //BDCSVD<mat<>> svd(A, ComputeFullU);
    JacobiSVD<mat<>> svd(A, ComputeFullU);
    return svd.matrixU() * svd.singularValues().cwiseSqrt().asDiagonal();    
}

// Sigma point generation
ukf::sig::sig(const filter::dist& distX, const filter::dist& distW, double k) :
        nx(distX.dim), nw(distW.dim), n(2*(nx + nw)+1),
        X(distX.dim, n), W(distW.dim, n), w(n) {

    double c, w0, ws, scale;
    c = nx + nw + k;
    w0 = k / c;
    ws = 0.5 / c;
    scale = sqrt(c);

    w(0) = w0;
    w.tail(n-1).setConstant(ws);

    mat<> Lxx = mat_sqrt(distX.cov);
    mat<> Lww = mat_sqrt(distW.cov);

    X = distX.mean.rowwise().replicate(n);
    W = distW.mean.rowwise().replicate(n);

    X.block(0, 1, nx, nx) += scale * Lxx;
    X.block(0, 1+nx, nx, nx) -= scale * Lxx;

    W.block(0, 2*nx+1,    nw, nw) += scale * Lww;
    W.block(0, 2*nx+1+nw, nw, nw) -= scale * Lww;

}
