#ifndef RANDO_HPP
#define RANDO_HPP

#include <mat.hpp>

#include <random>

class rando {

    private:
    
    // Mersenne twister
    std::mt19937_64 twister;

    // Standard normal distribution generator
    std::normal_distribution<double> normal_gen; 

    // Uniform distribution on [-1,1] generator
    std::uniform_real_distribution<double> uniform_gen;

    public:

    // Constructor
    rando(int seed = 0) :
        twister(seed), normal_gen(0, 1), uniform_gen(-1, 1) {}

    // Standard normal distribution
    double norm() {
        return normal_gen(twister);
    }

    // Uniform distribution on [-1,1]
    double unif() {
        return uniform_gen(twister);
    }

    // Multivariate normal distribution
    template <int N> vec<N> mvn() {
        vec<N> v;
        for (int i = 0; i < N; i++)
            v(i) = norm();
        return v;
    } 

    // Random unit vector (uniformly distributed)
    template <int N> vec<N> ruv() {
        vec<N> u;
        do {
            u = mvn<N>();
        } while (u.norm() < 1E-3);
        u.normalize();
        return u;
    }

    // Random quaternion (uniformly distributed axis)
    quat rq(double stda) {
        vec<3> a = ruv<3>();
        Eigen::AngleAxisd aa(stda * norm(), a);
        quat q(aa);
        return q;
    }

    // Variance of uniform distribution on [-1,1]
    static constexpr double unif_var = 0.33333333333333333333333333333; 

    // Kurtosis of uniform distribution
    static constexpr double unif_kurt = 1.8;

};

#endif
