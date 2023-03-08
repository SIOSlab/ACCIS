#include <sqrtukf.hpp>

#include <cmath>

using namespace filter;

// Filter prediction
dist sqrtukf::predict(double ti, double tf, const dist& distXi, const dist& distW,
        dyn& f) {

    sig S(distXi, distW, K);

    mat<> Xf(distXi.dim, S.n);

    for (int i = 0; i < S.n; i++)
        Xf.col(i) = f.f(ti, tf, S.X.col(i), S.W.col(i));

    dist distXf(distXi.dim);

    distXf.mean = Xf * S.w;

    mat<> Xfcv = (Xf.colwise() - distXf.mean) * S.v.asDiagonal();

    distXf.par.push_back(lmat(Xfcv));

    distXf.cov = distXf.par.front() * distXf.par.front().transpose(); 

    return distXf; 

}

// Filter update
dist sqrtukf::update(double t, cvec<> z, const dist& distXp, const dist& distW,
        meas& h) {

    using namespace Eigen;

    sig S(distXp, distW, K);

    mat<> Z(z.size(), S.n);

    for (int i = 0; i < S.n; i++)
        Z.col(i) = h.h(t, S.X.col(i), S.W.col(i));

    vec<> zm = Z * S.w;

    mat<> Zcv = (Z.colwise() - zm) * S.v.asDiagonal();

    mat<> Xcv = S.Xc * S.v.asDiagonal();

    JacobiSVD<mat<>> svd(Zcv.transpose(), ComputeThinU | ComputeThinV |
            FullPivHouseholderQRPreconditioner);
    
    mat<> Kt = svd.solve(Xcv.transpose());

    mat<> Xucv = Xcv - Kt.transpose() * Zcv;

    dist distXu(distXp.dim);

    distXu.mean = distXp.mean + Kt.transpose() * (z - zm);

    distXu.par.push_back(lmat(Xucv));

    distXu.cov = distXu.par.front() * distXu.par.front().transpose(); 

    return distXu; 

}

// Joint distribution from two independent distributions 
dist sqrtukf::join(const dist& dist1, const dist& dist2) {

    dist distc(dist1.dim + dist2.dim);

    distc.mean.head(dist1.dim) = dist1.mean;
    distc.mean.tail(dist2.dim) = dist2.mean;

    distc.cov.setZero();

    distc.cov.topLeftCorner(dist1.dim, dist1.dim) = dist1.cov;
    distc.cov.bottomRightCorner(dist2.dim, dist2.dim) = dist2.cov;

    distc.par.emplace_back(distc.dim, distc.dim);

    distc.par.front().setZero();

    distc.par.front().topLeftCorner(dist1.dim, dist1.dim) = chol_cov(dist1); 
    distc.par.front().bottomRightCorner(dist2.dim, dist2.dim) = chol_cov(dist2);

    return distc;

}

// Marginal distribution for distribution components
dist sqrtukf::marginal(const dist& joint_dist, int ind, int dim) {

    dist marg_dist(dim);

    marg_dist.mean = joint_dist.mean.segment(ind, dim);

    marg_dist.cov = joint_dist.cov.block(ind, ind, dim, dim);

    return marg_dist;

} 

// Cholesky decomposition of covariance
mat<> sqrtukf::chol_cov(const filter::dist& distX) {

    mat<> ll(distX.dim, distX.dim);

    if (distX.par.size() == 0)
        ll = distX.cov.llt().matrixL();
    else
        ll = distX.par.back();

    return ll;
}

// L matrix from LQ decomposition
mat<> sqrtukf::lmat(cmat<> A) {

    mat<> QR = A.transpose().householderQr().matrixQR();
        
    mat<> ll(A.rows(), A.rows());

    ll.setZero();

    ll = QR.topRows(A.rows()).transpose()
        .template triangularView<Eigen::Lower>();

    return ll;

}

// Sigma point generation
sqrtukf::sig::sig(const filter::dist& distX, const filter::dist& distW,
            double k) :
        nx(distX.dim), nw(distW.dim), n(2*(nx + nw)+1),
        X(nx, n), W(nw, n), Xc(nx, n), Wc(nw, n), w(n) {

    double c, w0, ws, scale;
    c = nx + nw + k;
    w0 = k / c;
    ws = 0.5 / c;
    scale = sqrt(c);

    w(0) = w0;
    w.tail(n-1).setConstant(ws);

    v = w.cwiseSqrt();

    mat<> Lxx = chol_cov(distX);
    mat<> Lww = distW.cov.llt().matrixL();

    Xc.setZero();
    Wc.setZero();
    
    Xc.block(0, 1,    nx, nx) += scale * Lxx; 
    Xc.block(0, 1+nx, nx, nx) -= scale * Lxx;

    Wc.block(0, 2*nx+1,    nw, nw) += scale * Lww;    
    Wc.block(0, 2*nx+1+nw, nw, nw) -= scale * Lww;

    X = Xc.colwise() + distX.mean;
    W = Wc.colwise() + distW.mean;

}
