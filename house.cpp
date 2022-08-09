#include <house.hpp>

#include <algorithm>
#include <cmath>

using namespace filter;

// Filter prediction
dist house::predict(double ti, double tf, const dist& distXi, const dist& distW,
        dyn& f) {

    sig S(distXi, distW, delta);

    mat<> Xf(distXi.dim, S.n);

    for (int i = 0; i < S.n; i++)
        Xf.col(i) = f.f(ti, tf, S.X.col(i), S.W.col(i));

    return get_dist(Xf, S.w); 

}

// Filter update
dist house::update(double t, cvec<> z, const dist& distXp, const dist& distW,
        meas& h) {

    using namespace Eigen;

    sig S(distXp, distW, delta);

    mat<> Z(z.size(), S.n);

    for (int i = 0; i < S.n; i++)
        Z.col(i) = h.h(t, S.X.col(i), S.W.col(i));

    vec<> zm = Z * S.w;

    mat<> Pzx = Z * S.w.asDiagonal() * S.X.transpose()
                    - zm * distXp.mean.transpose();

    mat<> Pzz = Z * S.w.asDiagonal() * Z.transpose() - zm * zm.transpose();

    BDCSVD<mat<>> Pzz_svd(Pzz, ComputeFullU | ComputeFullV);

    mat<> K = Pzz_svd.solve(Pzx).transpose();

    mat<> xu = S.X - K * (Z.colwise() - zm);

    dist dist_xu = get_dist(xu, S.w);

    dist_xu.mean = distXp.mean + K * (z - zm);

    bool upd_ok = !dist_xu.mean.hasNaN() && !dist_xu.cov.hasNaN();

    return upd_ok ? dist_xu : distXp;
    
}

// Joint distribution from two independent distributions 
dist house::join(const dist& dist1, const dist& dist2) {

    dist distc(dist1.dim + dist2.dim);

    distc.mean.head(dist1.dim) = dist1.mean;
    distc.mean.tail(dist2.dim) = dist2.mean;

    distc.cov.setZero();

    distc.cov.topLeftCorner(dist1.dim, dist1.dim) = dist1.cov;
    distc.cov.bottomRightCorner(dist2.dim, dist2.dim) = dist2.cov;

    vec<> skew(distc.dim), kurt(distc.dim);

    skew.head(dist1.dim) = dist1.par.at(0);
    skew.head(dist2.dim) = dist2.par.at(0);

    kurt.head(dist1.dim) = dist1.par.at(1);
    kurt.head(dist2.dim) = dist2.par.at(1);

    distc.par.push_back(skew);
    distc.par.push_back(kurt);

    return distc;

}

// Marginal distribution for distribution components
dist house::marginal(const dist& joint_dist, int ind, int dim) {

    dist marg_dist(dim);

    marg_dist.mean = joint_dist.mean.segment(ind, dim);

    marg_dist.cov = joint_dist.cov.block(ind, ind, dim, dim);

    // TO DO -- SKEWNESS & KURTOSIS

    return marg_dist;

} 

dist house::get_dist(cmat<> X, cvec<> w) {

    dist distX(X.rows());

    distX.mean = X * w;

    mat<> Xc = X.colwise() - distX.mean;

    distX.cov = Xc * w.asDiagonal() * Xc.transpose();

    //mat<> Xs = mat_sqrt(distX.cov).fullPivHouseholderQr().solve(Xc);

    mat<> Xs = mat_sqrt_solve(distX.cov, Xc);

    distX.par.push_back(Xs.array().pow(3).matrix() * w);
    distX.par.push_back(Xs.array().pow(4).matrix() * w);

    return distX;

}

// Matrix square root
mat<> house::mat_sqrt(cmat<> A) {
    using namespace Eigen;
    JacobiSVD<mat<>> svd(A, ComputeFullU);
    return svd.matrixU() * svd.singularValues().cwiseSqrt().asDiagonal();    
}

mat<> house::mat_sqrt_solve(cmat<> A, cmat<> B) {
    return mat_sqrt(A).fullPivHouseholderQr().solve(B);
}
    /*
    using namespace Eigen;
    BDCSVD<mat<>> svd(A, ComputeFullU);
    return svd.singularValues().cwiseSqrt().asDiagonal().inverse() 
        * svd.matrixU().transpose() * B;
*/


// Sigma point generation
house::sig::sig(const filter::dist& distX, const filter::dist& distW, 
            double delta) :
        nx(distX.dim), nw(distW.dim), n(2*(nx + nw)+1),
        X(distX.dim, n), W(distW.dim, n), w(n) {

    vec<> sx, kx, sw, kw, ax, bx, cx, aw, bw, cw;

    sx = distX.par.at(0);
    sw = distW.par.at(0);
    kx = distX.par.at(1);
    kw = distW.par.at(1);
    
    double kmin = (nx + nw) / (1 - delta);

    for (int i = 0; i < nx; i++)
        kx(i) = std::max(kx(i), kmin);
    for (int i = 0; i < nw; i++)
        kw(i) = std::max(kw(i), kmin);

    cx = (4 * kx.array() - 3 * sx.array().square()).sqrt();
    cw = (4 * kw.array() - 3 * sw.array().square()).sqrt();

    ax = (sx + cx) / 2;
    aw = (sw + cw) / 2;

    bx = ax - sx;
    bw = aw - sw;

    w.segment(1, nx)    = ax.cwiseProduct(cx).cwiseInverse();
    w.segment(1+nx, nx) = bx.cwiseProduct(cx).cwiseInverse();

    w.segment(1+2*nx, nw)    = aw.cwiseProduct(cw).cwiseInverse();
    w.segment(1+2*nx+nw, nw) = bw.cwiseProduct(cw).cwiseInverse();

    w(0) = 1 - (kx.array() - sx.array().square()).inverse().sum() 
             - (kw.array() - sw.array().square()).inverse().sum();

    X = distX.mean.rowwise().replicate(n);
    W = distW.mean.rowwise().replicate(n);

    mat<> sqrt_cov_x = mat_sqrt(distX.cov);
    mat<> sqrt_cov_w = mat_sqrt(distW.cov);

    X.block(0, 1,    nx, nx) += sqrt_cov_x * ax.asDiagonal();
    X.block(0, 1+nx, nx, nx) -= sqrt_cov_x * bx.asDiagonal();

    W.block(0, 1+2*nx,    nw, nw) += sqrt_cov_w * aw.asDiagonal();
    W.block(0, 1+2*nx+nw, nw, nw) -= sqrt_cov_w * bw.asDiagonal();
        
}
