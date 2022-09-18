#include "numder.hpp"

using namespace numder;

mat<> numder::jacobian(const fun& g, cvec<> x, cvec<> dx) {

    

}

vec<> numder::lieder(const fun& f, const fun& h, cvec<> x, cvec<> dx) {

    return jacobian(h, x, dx) * f(x);

}

vec<> numder::lieder_n(const fun& f, const fun& h, cvec<> x, cvec<> dx,
        unsigned int n) {

    if (n == 0) {

        return h(x);

    } else if (n == 1) {

        return lieder(f, h, x, dx);

    } else {

        fun hh = [n, &f, &h, &dx] (cvec<> xx) -> vec<> {
            return lieder_n(f, h, xx, dx, n-1);  
        };

        return lieder(f, hh, x, dx);

    }

}
