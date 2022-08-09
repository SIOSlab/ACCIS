#ifndef UKF_HPP
#define UKF_HPP

#include <filter.hpp>

//------------------------------------------------------------------------------
//  Unscented Kalman filter
//------------------------------------------------------------------------------
class ukf : public filter::base {

    public:

    // Tuning parameter
    const double K;

    // Constructor
    ukf(double K_ = 1) : K(K_) {}

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
    
    // Matrix square root
    static mat<> mat_sqrt(cmat<> A);

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

        // Weights
        vec<> w;

        // Constructor
        sig(const filter::dist& distX, const filter::dist& distW, double k);

    };

};

#endif
