#ifndef NUMDER_HPP
#define NUMDER_HPP

#include <functional>

#include "mat.hpp"

namespace numder {

    using fun = std::function<vec<>(vec<>)>;

    mat<> jacobian(const fun& g, cvec<> x, cvec<> dx);

    vec<> lieder(const fun& f, const fun& h, cvec<> x, cvec<> dx);

    vec<> lieder_n(const fun& f, const fun& h, cvec<> x, cvec<> dx,
            unsigned int n);

}

#endif
