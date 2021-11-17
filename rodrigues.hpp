#ifndef RODRIGUES_HPP
#define RODRIGUES_HPP

#include "mat.hpp"

inline vec<3> quat2rod(const quat& q) {
    return q.vec() / q.w();
}

inline quat rod2quat(cvec<3> p) {
    quat q;
    q.vec() = p;
    q.w() = 1;
    q.normalize();
    return q;
}

#endif
