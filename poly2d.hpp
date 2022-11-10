#ifndef POLB2D_HPP
#define POLB2D_HPP

#include "mat.hpp"

#include <cmath>
#include <vector>

namespace poly2d {

    using namespace Eigen;

    constexpr int deg = 3;

    constexpr int np = (deg + 1) * (deg + 1);

    inline mat<np,2> fit(const std::vector<vec<2>>& x,
            const std::vector<vec<2>>& y) {

        mat<> B(y.size(), 2);

        for (int i = 0; i < y.size(); i++)
           B.row(i) = y[i];

        mat<> A(x.size(), np);

        for (int i = 0; i < x.size(); i++) {

            double r = x[i].norm();
            double f = atan2(x[i].y(), x[i].x());

            mat<deg+1,deg+1> Ai;
            
            Ai(0, 0) = 1;

            for (int n = 1; n <= deg; n++) {

                Ai(n, n) = Ai(n-1, n-1) * r;

                for (int m = 1; m <= n; m++) {
                    Ai(n, m-1) = Ai(n, n) * cos(m * f);
                    Ai(m-1, n) = Ai(n, n) * sin(m * f); 
                }

            } 

            A.row(i) = Ai.reshaped();

        }
        
        JacobiSVD<mat<>> svd(A, ComputeThinU | ComputeThinV);

        return svd.solve(B);

    } 

}

#endif
