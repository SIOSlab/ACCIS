#ifndef SQRTUKF_HPP
#define SQRTUKF_HPP

#include <filter.hpp>

//------------------------------------------------------------------------------
//  Square-Root Unscented Kalman filter
//------------------------------------------------------------------------------
class sqrtukf : public filter::base {

    public:

    // Tuning parameter
    const double K;

    // Constructor
    sqrtukf(double K_ = 1) : K(K_) {}

    // Filter prediction
    virtual filter::dist predict(double ti, double tf,
                const filter::dist& distXi, const filter::dist& distW,
                filter::dyn& f);

    // Filter update
    virtual filter::dist update(double ti, cvec<> z, const filter::dist& distX,
                const filter::dist& distW, filter::meas& h);

    // Joint distribution from two independent distributions 
    virtual filter::dist join(const filter::dist& dist1,
                const filter::dist& dist2);

    // Marginal distribution for distribution components
    virtual filter::dist marginal(const filter::dist& joint_dist, int ind,
                int dim); 

    // Cholesky decomposition of covariance
    static mat<> chol_cov(const filter::dist& distX);

    // L matrix from LQ decomposition
    static mat<> lmat(cmat<> A);

    //--------------------------------------------------------------------------
    //  Sigma point structure
    //--------------------------------------------------------------------------
    struct sig {

        // Dimensions of state & noise
        int nx, nw;
            
        // Number of sigma points
        int n;

        // State sigma points
        mat<> X;

        // Noise sigma points
        mat<> W;

        // Centralized state sigma points
        mat<> Xc;
        
        // Centralized noise sigma points
        mat<> Wc;

        // Weights
        vec<> w;

        // Square roots of weights
        vec<> v;

        // Constructor
        sig(const filter::dist& distX, const filter::dist& distW, double k);

    };

};

#endif
