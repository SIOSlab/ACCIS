#ifndef FILTER_HPP
#define FILTER_HPP

#include <functional>
#include <vector>

#include <mat.hpp>

namespace filter {

    //--------------------------------------------------------------------------
    //  Statistical Distribution Type
    //--------------------------------------------------------------------------
    struct dist {

        // Dimension
        int dim;

        // Mean vector
        vec<> mean;

        // Covariance matrix
        mat<> cov;

        // Other parameters (e.g, higher-order moments) 
        std::vector<mat<>> par;

        // Constructor
        dist(int dim_ = 0) : dim(dim_), mean(dim), cov(dim, dim) {}

    };

    //--------------------------------------------------------------------------
    //  Filtering Function Types
    //--------------------------------------------------------------------------
    
    // Noisy dynamical model: xf = f(ti, tf, xi, w)
    class dyn {

        public:

        virtual vec<> f(double ti, double tf, cvec<> xi, cvec<> w) = 0;
    
    };

    // Noisy measurement model: z = h(t, x, w)
    class meas {

        public:

        virtual vec<> h(double t, cvec<> x, cvec<> w) = 0;

    };

    //--------------------------------------------------------------------------
    //  Abstract Base Class for Filters
    //--------------------------------------------------------------------------
    class base {

        public:

        // Filter prediction
        virtual dist predict(double ti, double tf, const dist& dist_xi,
                    const dist& dist_w, dyn& f) = 0;

                
        // Filter update
        virtual dist update(double t, cvec<> z, const dist& dist_x,
                    const dist& dist_w, meas& h) = 0;

        // Joint distribution from two independent distributions 
        virtual dist join(const dist& dist1, const dist& dist2) = 0;

        // Standard normal distribution
        //virtual dist normal(int dim) = 0;

        // Uniform distribution on [-1,1]
        //virtual dist unif(int dim) = 0;

    };

}

#endif
