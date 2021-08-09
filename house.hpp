#ifndef HOUSE_HPP
#define HOUSE_HPP

#include <filter.hpp>

//------------------------------------------------------------------------------
//  Higher-Order Unscented Estimator
//------------------------------------------------------------------------------
class house : public filter::base {

    public:

    // Tuning parameter
    const double delta;

    // Constructor
    house(double delta_ = 0) : delta(delta_) {}

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

    // Distribution from points & weights
    static filter::dist get_dist(cmat<> X, cvec<> w);

    // Matrix square root
    static mat<> mat_sqrt(cmat<> A);

    // Solve sqrt(A)X = B for X
    static mat<> mat_sqrt_solve(cmat<> A, cmat<> B);

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
        sig(const filter::dist& distX, const filter::dist& distW, double delta);

    };

};

#endif
