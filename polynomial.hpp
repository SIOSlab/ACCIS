#ifndef POLYNOMIAL_HPP
#define POLYNOMIAL_HPP

#include <Eigen/Core>

#include <cmath>

namespace polynomial {

    // Evaluate polynomial using Horner's scheme
    template <typename T> double eval(const Eigen::DenseBase<T>& c, double x) {
        double y = 0;
        for (int i = c.size()-1; i >= 0; i--)
            y = c(i) + y * x;
        return y;
    }

    // Evaluate polynomial derivative using Horner's scheme
    template <typename T> double deriv(const Eigen::DenseBase<T>& c, double x) {
        double yd = 0;
        for (int i = c.size()-1; i > 0; i--)
            yd = (i+1) * c(i) + yd * x;
        return yd;
    }

    // Find root of polynomial using Newton's method
    template <typename T> double solve(const Eigen::DenseBase<T>& c,
            double x, double tol) {
        double dx;
        int it = 0;
        do {
            dx = eval(c, x) / deriv(c, x);
            x -= dx;
            it++;
        } while (fabs(dx) > tol && it < 10);
        return x;
    }

}

#endif
